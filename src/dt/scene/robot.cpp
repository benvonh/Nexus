#include "dt/scene/robot.hpp"
#include "dt/scene/manager.hpp"
#include "pxr/usd/usdGeom/cube.h"
#include "pxr/usd/usdGeom/cylinder.h"
#include "pxr/usd/usdGeom/sphere.h"
#include "urdf/model.h"
#include <iostream>
#include <memory>

namespace dt
{
namespace scene
{
Robot::Robot()
{
  // auto stage = permit.stage;

  // auto axis = pxr::UsdGeomCylinder::Define(stage, pxr::SdfPath("/axis"));
  // auto leg1 = pxr::UsdGeomCylinder::Define(stage, pxr::SdfPath("/leg1"));
  // auto leg2 = pxr::UsdGeomCylinder::Define(stage, pxr::SdfPath("/leg2"));
  // auto body = pxr::UsdGeomCylinder::Define(stage, pxr::SdfPath("/body"));
  // auto head = pxr::UsdGeomSphere::Define(stage, pxr::SdfPath("/head"));
  // auto rod = pxr::UsdGeomCylinder::Define(stage, pxr::SdfPath("/rod"));
  // auto box = pxr::UsdGeomCube::Define(stage, pxr::SdfPath("/box"));

  // this->xforms["axis"] = this->axis.AddTransformOp();
  // this->xforms["leg1"] = this->leg1.AddTransformOp();
  // this->xforms["leg2"] = this->leg2.AddTransformOp();
  // this->xforms["body"] = this->body.AddTransformOp();
  // this->xforms["head"] = this->head.AddTransformOp();
  // this->xforms["rod"] = this->rod.AddTransformOp();
  // this->xforms["box"] = this->box.AddTransformOp();

  // auto axis.CreateHeightAttr(pxr::VtValue(0.5));
  // auto axis.CreateRadiusAttr(pxr::VtValue(0.01));
  // auto axis.CreateDisplayColorPrimvar().Set(pxr::VtArray(pxr::GfVec3f(0.2, 0.2, 0.2)));
  // auto axis.AddRotateXOp().Set(pxr::VtValue(90.0f));

  // auto leg1.CreateHeightAttr(pxr::VtValue(0.8));
  // auto leg1.CreateRadiusAttr(pxr::VtValue(0.1));
  // auto leg1.CreateDisplayColorPrimvar().Set(pxr::VtArray(pxr::GfVec3f(0.5, 0.5, 0.5)));
  // auto leg1.AddTranslateZOp().Set(pxr::VtValue(-0.3));

  // auto leg2.CreateHeightAttr(pxr::VtValue(0.8));
  // auto leg2.CreateRadiusAttr(pxr::VtValue(0.1));
  // auto leg2.CreateDisplayColorPrimvar().Set(pxr::VtArray(pxr::GfVec3f(0.5, 0.5, 0.5)));
  // auto leg2.AddTranslateZOp().Set(pxr::VtValue(-0.3));

  // auto body.CreateHeightAttr(pxr::VtValue(0.6));
  // auto body.CreateRadiusAttr(pxr::VtValue(0.2));
  // auto body.CreateDisplayColorPrimvar().Set(pxr::VtArray(pxr::GfVec3f(0.4, 0.4, 0.4)));
  // auto body.AddTranslateZOp().Set(pxr::VtValue(-0.2));

  // auto head.CreateRadiusAttr(pxr::VtValue(0.4));
  // auto head.CreateDisplayColorPrimvar().Set(pxr::VtArray(pxr::GfVec3f(0.2, 0.2, 0.8)));

  // auto rod.CreateHeightAttr(pxr::VtValue(0.2));
  // auto rod.CreateRadiusAttr(pxr::VtValue(0.02));
  // auto rod.CreateDisplayColorPrimvar().Set(pxr::VtArray(pxr::GfVec3f(0.1, 0.1, 0.1)));
  // auto rod.AddTranslateZOp().Set(pxr::VtValue(-0.1));

  // auto box.CreateSizeAttr(pxr::VtValue(0.05));
  // auto box.CreateDisplayColorPrimvar().Set(pxr::VtArray(pxr::GfVec3f(0.8, 0.3, 0.3)));
}

void Robot::ParseURDF(const std::string &path)
{
  urdf::Model model;

  if (!model.initFile(path))
    throw std::exception("URDF parse failure");

  std::cout << "Done parsing robot (" << model.name_ << ")\n";

  auto permit = scene::Manager::GetStagePermit();
  auto &stage = permit.stage;

  const pxr::SdfPath root(std::string("/") + model.name_);
  stage->DefinePrim(root);

  for (const auto &[name, link] : model.links_)
  {
    urdf::VisualSharedPtr &visual = link->visual;
    urdf::GeometrySharedPtr &geometry = visual->geometry;
    urdf::Pose &origin = visual->origin;
    
    switch (geometry->type)
    {
    case urdf::Geometry::BOX:
    {
      auto box = std::static_pointer_cast<urdf::Box>(geometry);
      auto cube = pxr::UsdGeomCube::Define(stage, root.AppendChild(pxr::TfToken(name)));
      cube.CreateSizeAttr(pxr::VtValue(box->dim.x)); // FIXME: cube only has a size
      cube.CreateDisplayColorPrimvar().Set(pxr::VtArray(pxr::GfVec3f(0.5, 0.5, 0.5)));
      this->xforms[name] = cube.AddTransformOp();
      break;
    }
    case urdf::Geometry::CYLINDER:
    {
      auto cylinder = std::static_pointer_cast<urdf::Cylinder>(geometry);
      auto usd_cylinder = pxr::UsdGeomCylinder::Define(stage, root.AppendChild(pxr::TfToken(name)));
      usd_cylinder.CreateHeightAttr(pxr::VtValue(cylinder->length));
      usd_cylinder.CreateRadiusAttr(pxr::VtValue(cylinder->radius));
      usd_cylinder.CreateDisplayColorPrimvar().Set(pxr::VtArray(pxr::GfVec3f(0.5, 0.5, 0.5)));
      this->xforms[name] = usd_cylinder.AddTransformOp();
      break;
    }
    case urdf::Geometry::MESH:
      std::cerr << "Unsupported\n";
      break;
    case urdf::Geometry::SPHERE:
    {
      auto sphere = std::static_pointer_cast<urdf::Sphere>(geometry);
      auto usd_sphere = pxr::UsdGeomSphere::Define(stage, root.AppendChild(pxr::TfToken(name)));
      usd_sphere.CreateRadiusAttr(pxr::VtValue(sphere->radius));
      usd_sphere.CreateDisplayColorPrimvar().Set(pxr::VtArray(pxr::GfVec3f(0.5, 0.5, 0.5)));
      this->xforms[name] = usd_sphere.AddTransformOp();
      break;
    }
    default:
      std::cerr << "Unknown geometry type\n";
    }
  }
}

} // namespace scene
} // namespace dt