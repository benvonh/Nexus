#include "robot.h"

#include "nexus/logging.h"

#include "pxr/usd/usdGeom/cube.h"
#include "pxr/usd/usdGeom/cylinder.h"
#include "pxr/usd/usdGeom/mesh.h"
#include "pxr/usd/usdGeom/sphere.h"
#include "pxr/usd/usdGeom/xform.h"

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"

#include "urdf/model.h"

// FIXME: Very messy URDF to USD!

/*============================================================================*/
static void CreateMesh(const aiScene *scene, const aiNode *node, const pxr::SdfPath &path, pxr::UsdStageRefPtr stage)
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
            Nexus::log::debug("Sanitized mesh name from '{}' to '{}'", mesh->mName.C_Str(), meshName);
        }
        else
        {
            Nexus::log::debug("Got mesh {}", meshName);
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
        Nexus::log::debug("Defined USD mesh at {}", usdMesh.GetPath().GetString());
    }

    for (int i = 0; i < node->mNumChildren; i++)
    {
        CreateMesh(scene, node->mChildren[i], nodePath, stage);
    }
}

/*============================================================================*/
void Nexus::Robot::reset()
{
    _XformOps.clear();
}

/*============================================================================*/
void Nexus::Robot::from_urdf(const std::string &path)
{
    urdf::Model model;

    if (!model.initFile(path))
    {
        log::error("Could not parse URDF at {}", path);
        return;
    }

    log::event("Parsed URDF with robot name {}", model.name_);
    const pxr::SdfPath root('/' + model.name_);
    _XformOps.clear();

    auto [stage, _] = Nexus::World::GetStagePermit();

    for (const auto &[name, link] : model.links_)
    {
        const pxr::SdfPath linkPath = root.AppendChild(pxr::TfToken(name));
        auto xform = pxr::UsdGeomXform::Define(stage, linkPath);

        log::debug("Got link '{}'", name);

        if (!link)
        {
            log::debug("Link was null... skipping");
            continue;
        }
        const urdf::VisualSharedPtr &visual = link->visual;

        if (!visual)
        {
            log::debug("Visual was null... skipping");
            continue;
        }
        const urdf::GeometrySharedPtr &geometry = visual->geometry;

        if (!geometry)
        {
            log::debug("Geometry was null... skipping");
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
        _XformOps[name] = xform.AddTransformOp();

        switch (geometry->type)
        {
        case urdf::Geometry::BOX:
        {
            // auto box = std::static_pointer_cast<urdf::Box>(geometry);
            // auto cube = pxr::UsdGeomCube::Define(stage, root.AppendChild(pxr::TfToken(name)));
            // cube.CreateSizeAttr(pxr::VtValue(box->dim.x)); // FIXME: cube only has a size
            // cube.CreateDisplayColorPrimvar().Set(pxr::VtArray(pxr::GfVec3f(0.5, 0.5, 0.5)));
            // _XformOps[name] = cube.AddTransformOp();
            log::alert("Box geometry is not supported");
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
            log::alert("Cylinder geometry is not supported");
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
                log::debug("Could not import mesh at {}", urdfMesh->filename);
                break;
            }
            log::debug("Imported mesh at {}", urdfMesh->filename);

            if (scene->mNumMeshes == 0)
            {
                log::debug("Mesh was empty... skipping");
                break;
            }

            for (int i = 0; i < scene->mRootNode->mNumChildren; i++)
            {
                const aiNode *child = scene->mRootNode->mChildren[i];
                if (child->mNumMeshes > 0)
                {
                    log::debug("Found mesh node '{}'", child->mName.C_Str());
                    CreateMesh(scene, child, linkPath, stage);
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
            log::alert("Sphere geometry is not supported");
            break;
        }
        default:
            log::error("Unknown geometry type!");
        }
    }
    log::event("Converted URDF to USD with {} links", _XformOps.size());
}