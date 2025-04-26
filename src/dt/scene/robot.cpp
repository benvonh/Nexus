#include "dt/scene/robot.hpp"
#include "dt/scene/manager.hpp"

namespace dt
{
namespace scene
{
Robot::Robot(Permit<pxr::UsdStageRefPtr> &&permit)
{
  auto stage = permit.stage;

  this->axis = pxr::UsdGeomCylinder::Define(stage, pxr::SdfPath("/axis"));
  this->leg1 = pxr::UsdGeomCylinder::Define(stage, pxr::SdfPath("/leg1"));
  this->leg2 = pxr::UsdGeomCylinder::Define(stage, pxr::SdfPath("/leg2"));
  this->body = pxr::UsdGeomCylinder::Define(stage, pxr::SdfPath("/body"));
  this->head = pxr::UsdGeomSphere::Define(stage, pxr::SdfPath("/head"));
  this->rod = pxr::UsdGeomCylinder::Define(stage, pxr::SdfPath("/rod"));
  this->box = pxr::UsdGeomCube::Define(stage, pxr::SdfPath("/box"));

  xforms["axis"] = this->axis.AddTransformOp();
  xforms["leg1"] = this->leg1.AddTransformOp();
  xforms["leg2"] = this->leg2.AddTransformOp();
  xforms["body"] = this->body.AddTransformOp();
  xforms["head"] = this->head.AddTransformOp();
  xforms["rod"] = this->rod.AddTransformOp();
  xforms["box"] = this->box.AddTransformOp();

  this->axis.CreateHeightAttr(pxr::VtValue(0.5));
  this->axis.CreateRadiusAttr(pxr::VtValue(0.01));
  this->axis.CreateDisplayColorPrimvar().Set(pxr::VtArray(pxr::GfVec3f(0.2, 0.2, 0.2)));
  this->axis.AddRotateXOp().Set(pxr::VtValue(90.0f));

  this->leg1.CreateHeightAttr(pxr::VtValue(0.8));
  this->leg1.CreateRadiusAttr(pxr::VtValue(0.1));
  this->leg1.CreateDisplayColorPrimvar().Set(pxr::VtArray(pxr::GfVec3f(0.5, 0.5, 0.5)));
  this->leg1.AddTranslateZOp().Set(pxr::VtValue(-0.3));

  this->leg2.CreateHeightAttr(pxr::VtValue(0.8));
  this->leg2.CreateRadiusAttr(pxr::VtValue(0.1));
  this->leg2.CreateDisplayColorPrimvar().Set(pxr::VtArray(pxr::GfVec3f(0.5, 0.5, 0.5)));
  this->leg2.AddTranslateZOp().Set(pxr::VtValue(-0.3));

  this->body.CreateHeightAttr(pxr::VtValue(0.6));
  this->body.CreateRadiusAttr(pxr::VtValue(0.2));
  this->body.CreateDisplayColorPrimvar().Set(pxr::VtArray(pxr::GfVec3f(0.4, 0.4, 0.4)));
  this->body.AddTranslateZOp().Set(pxr::VtValue(-0.2));

  this->head.CreateRadiusAttr(pxr::VtValue(0.4));
  this->head.CreateDisplayColorPrimvar().Set(pxr::VtArray(pxr::GfVec3f(0.2, 0.2, 0.8)));

  this->rod.CreateHeightAttr(pxr::VtValue(0.2));
  this->rod.CreateRadiusAttr(pxr::VtValue(0.02));
  this->rod.CreateDisplayColorPrimvar().Set(pxr::VtArray(pxr::GfVec3f(0.1, 0.1, 0.1)));
  this->rod.AddTranslateZOp().Set(pxr::VtValue(-0.1));

  this->box.CreateSizeAttr(pxr::VtValue(0.05));
  this->box.CreateDisplayColorPrimvar().Set(pxr::VtArray(pxr::GfVec3f(0.8, 0.3, 0.3)));
}
} // namespace scene
} // namespace dt