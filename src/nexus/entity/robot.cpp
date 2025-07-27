#include "robot.h"

#include "nexus/core/world.h"

#include "pxr/usd/usdGeom/cube.h"
#include "pxr/usd/usdGeom/cylinder.h"
#include "pxr/usd/usdGeom/mesh.h"
#include "pxr/usd/usdGeom/sphere.h"
#include "pxr/usd/usdGeom/xform.h"
#include "pxr/base/gf/matrix4d.h"
#include "pxr/base/gf/quatd.h"
#include "pxr/base/gf/rotation.h"
#include "pxr/base/gf/vec3d.h"

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"

#include "urdf/model.h"

#include <chrono>

GENERATE_LOG_FUNCTIONS(URDF_to_USD)


// TODO: My god this is so complex

void CreateMesh(const aiScene *scene, const aiNode *node, const pxr::SdfPath &path, pxr::UsdStageRefPtr stage)
{
    auto nodePath = path.AppendChild(pxr::TfToken(node->mName.C_Str()));
    auto nodeXform = pxr::UsdGeomXform::Define(stage, nodePath);

    pxr::GfMatrix4d pxrTransform(
        node->mTransformation.a1, node->mTransformation.a2, node->mTransformation.a3, node->mTransformation.a4,
        node->mTransformation.b1, node->mTransformation.b2, node->mTransformation.b3, node->mTransformation.b4,
        node->mTransformation.c1, node->mTransformation.c2, node->mTransformation.c3, node->mTransformation.c4,
        node->mTransformation.d1, node->mTransformation.d2, node->mTransformation.d3, node->mTransformation.d4);
    pxrTransform.Orthonormalize();
    // pxrTransform.SetScale(100.f);

    nodeXform.AddTransformOp().Set(pxrTransform);
    // usdMesh.AddScaleOp().Set(0.001f);

    for (int i = 0; i < node->mNumMeshes; i++)
    {
        const aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        std::string meshName = mesh->mName.C_Str();
        std::replace(meshName.begin(), meshName.end(), '-', '_');

        if (meshName != mesh->mName.C_Str())
        {
            LOG_BASIC_URDF_to_USD("Sanitized mesh name from '{}' to '{}'", mesh->mName.C_Str(), meshName);
        }
        else
        {
            LOG_BASIC_URDF_to_USD("Got mesh {}", meshName);
        }

        pxr::VtArray<pxr::GfVec3f> points(mesh->mNumVertices);
        pxr::VtArray<pxr::GfVec3f> normals(mesh->mNumVertices);
        pxr::VtArray<int> faceVertexCounts(mesh->mNumFaces);
        pxr::VtArray<int> faceVertexIndices(mesh->mNumFaces * 3);

        for (int i = 0; i < points.size(); i++)
        {
            points[i] = pxr::GfVec3f(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
            normals[i] = pxr::GfVec3f(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
        }

        for (int i = 0; i < mesh->mNumFaces; i++)
        {
            const aiFace &face = mesh->mFaces[i];
            faceVertexCounts[i] = face.mNumIndices;

            for (int j = 0; j < face.mNumIndices; j++)
            {
                faceVertexIndices[i * 3 + j] = face.mIndices[j];
            }
        }

        auto usdMesh = pxr::UsdGeomMesh::Define(stage, nodePath.AppendChild(pxr::TfToken(meshName)));
        usdMesh.CreatePointsAttr(pxr::VtValue(points));
        usdMesh.CreateNormalsAttr(pxr::VtValue(normals));
        usdMesh.CreateFaceVertexCountsAttr(pxr::VtValue(faceVertexCounts));
        usdMesh.CreateFaceVertexIndicesAttr(pxr::VtValue(faceVertexIndices));
        LOG_BASIC_URDF_to_USD("Defined USD mesh at {}", usdMesh.GetPath().GetString());
    }

    for (int i = 0; i < node->mNumChildren; i++)
    {
        CreateMesh(scene, node->mChildren[i], nodePath, stage);
    }
}

Nexus::Robot::Robot(const std::filesystem::path &urdf_path) : Entity("Robot"), c_URDF_Path(urdf_path)
{
    using namespace std::chrono_literals;
    m_Buffer = std::make_unique<TF_Buffer>(this->get_clock());
    m_Listener = std::make_shared<TF_Listener>(m_Buffer.get());
    m_Timer = this->create_wall_timer(
        3ms, [this]()
        {
            try
            {
                auto time = World::GetTime();
                auto data = _get_write_access<Data>();

                for (auto &[name, xform] : *data)
                {
                    const auto look = m_Buffer->lookupTransform("base", name.c_str(), tf2::TimePointZero);
                    const auto &rotation = look.transform.rotation;
                    const auto &translation = look.transform.translation;
                    pxr::GfQuatd q(rotation.w, rotation.x, rotation.y, rotation.z);
                    pxr::GfVec3d t(translation.x, translation.y, translation.z);
                    pxr::GfMatrix4d m(q, t);
                    xform.Set(m, time);
                }
            }
            catch (const tf2::TransformException &e)
            {
                RCLCPP_ERROR_THROTTLE(this->get_logger(), *this->get_clock(), 1000, e.what());
            } });
}

Nexus::Entity::Data *Nexus::Robot::_create_data()
{
    urdf::Model model;

    if (!model.initFile(c_URDF_Path.string()))
    {
        LOG_ERROR("Could not parse URDF at {}", c_URDF_Path.string());
        return;
    }

    LOG_EVENT_URDF_to_USD("Parsed URDF with robot name {}", model.name_);
    const pxr::SdfPath root('/' + model.name_);

    auto *data = new Data();
    auto &xforms = *data;
    xforms.clear();

    auto stage = World::GetStageWriteAccess();

    for (const auto &[name, link] : model.links_)
    {
        const pxr::SdfPath linkPath = root.AppendChild(pxr::TfToken(name));
        auto xform = pxr::UsdGeomXform::Define(*stage, linkPath);

        LOG_BASIC("Got link '{}'", name);

        if (!link)
        {
            LOG_BASIC("Link was null... skipping");
            continue;
        }
        const urdf::VisualSharedPtr &visual = link->visual;

        if (!visual)
        {
            LOG_BASIC("Visual was null... skipping");
            continue;
        }
        const urdf::GeometrySharedPtr &geometry = visual->geometry;

        if (!geometry)
        {
            LOG_BASIC("Geometry was null... skipping");
            continue;
        }

        /* Apply visual offset */
        // TODO: right order?
        xform.AddTranslateOp().Set(pxr::GfVec3d(visual->origin.position.x,
                                                visual->origin.position.y,
                                                visual->origin.position.z));
        xform.AddOrientOp().Set(pxr::GfQuatf(visual->origin.rotation.w,
                                             visual->origin.rotation.x,
                                             visual->origin.rotation.y,
                                             visual->origin.rotation.z));
        xforms[name] = xform.AddTransformOp();

        switch (geometry->type)
        {
        case urdf::Geometry::BOX:
        {
            // auto box = std::static_pointer_cast<urdf::Box>(geometry);
            // auto cube = pxr::UsdGeomCube::Define(stage, root.AppendChild(pxr::TfToken(name)));
            // cube.CreateSizeAttr(pxr::VtValue(box->dim.x)); // FIXME: cube only has a size
            // cube.CreateDisplayColorPrimvar().Set(pxr::VtArray(pxr::GfVec3f(0.5, 0.5, 0.5)));
            // _XformOps[name] = cube.AddTransformOp();
            LOG_ALERT("Box geometry is not supported");
            break;
        }
        case urdf::Geometry::CYLINDER:
        {
            // auto cylinder = std::static_pointer_cast<urdf::Cylinder>(geometry);
            // auto usd_cylinder = pxr::UsdGeomCylinder::Define(stage, root.AppendChild(pxr::TfToken(name)));
            // usd_cylinder.CreateHeightAttr(pxr::VtValue(cylinder->length));
            // usd_cylinder.CreateRadiusAttr(pxr::VtValue(cylinder->radius));
            // usd_cylinder.CreateDisplayColorPrimvar().Set(pxr::VtArray(pxr::GfVec3f(0.5, 0.5, 0.5)));
            // _XformOps[name] = usd_cylinder.AddTransformOp();
            LOG_ALERT("Cylinder geometry is not supported");
            break;
        }
        case urdf::Geometry::MESH:
        {
            const auto &urdfMesh = std::static_pointer_cast<urdf::Mesh>(geometry);

            Assimp::Importer importer;

            importer.SetPropertyFloat(AI_CONFIG_GLOBAL_SCALE_FACTOR_KEY, 0.001f);

            const int pFlags = aiProcess_CalcTangentSpace |
                               aiProcess_Triangulate |
                               aiProcess_JoinIdenticalVertices |
                               aiProcess_SortByPType |
                               aiProcess_GlobalScale;
            const aiScene *scene = importer.ReadFile(urdfMesh->filename.c_str(), pFlags);

            if (scene == nullptr)
            {
                LOG_BASIC("Could not import mesh at {}", urdfMesh->filename);
                break;
            }
            LOG_BASIC("Imported mesh at {}", urdfMesh->filename);

            if (scene->mNumMeshes == 0)
            {
                LOG_BASIC("Mesh was empty... skipping");
                break;
            }

            for (int i = 0; i < scene->mRootNode->mNumChildren; i++)
            {
                const aiNode *child = scene->mRootNode->mChildren[i];
                if (child->mNumMeshes > 0)
                {
                    LOG_BASIC("Found mesh node '{}'", child->mName.C_Str());
                    CreateMesh(scene, child, linkPath, *stage);
                }
            }
            // CreateMesh(scene, scene->mRootNode, linkPath, stage);
            break;
        }
        case urdf::Geometry::SPHERE:
        {
            // auto sphere = std::static_pointer_cast<urdf::Sphere>(geometry);
            // auto usd_sphere = pxr::UsdGeomSphere::Define(stage, root.AppendChild(pxr::TfToken(name)));
            // usd_sphere.CreateRadiusAttr(pxr::VtValue(sphere->radius));
            // usd_sphere.CreateDisplayColorPrimvar().Set(pxr::VtArray(pxr::GfVec3f(0.5, 0.5, 0.5)));
            // _XformOps[name] = usd_sphere.AddTransformOp();
            LOG_ALERT("Sphere geometry is not supported");
            break;
        }
        default:
            LOG_ERROR("Unknown geometry type!");
        }
    }
    LOG_EVENT("Converted URDF to USD with {} links", xforms.size());
    return data;
}