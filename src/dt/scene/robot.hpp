#pragma once
#include "pxr/usd/usdGeom/cube.h"
#include "pxr/usd/usdGeom/cylinder.h"
#include "pxr/usd/usdGeom/sphere.h"
#include "pxr/usd/usdGeom/xformOp.h"

// FIXME:
// THIS IS A HARDCODED ROBOT OF THE R2D2 ROS EXAMPLE
namespace dt
{
namespace scene
{
template <typename T>
struct Permit;

struct Robot
{
  Robot(Permit<pxr::UsdStageRefPtr> &&permit);
  ~Robot() = default;
  Robot(Robot &&) = delete;
  Robot(const Robot &) = delete;
  Robot &operator=(Robot &&) = delete;
  Robot &operator=(const Robot &) = delete;

  pxr::UsdGeomCylinder axis;
  pxr::UsdGeomCylinder leg1;
  pxr::UsdGeomCylinder leg2;
  pxr::UsdGeomCylinder body;
  pxr::UsdGeomSphere head;
  pxr::UsdGeomCylinder rod;
  pxr::UsdGeomCube box;
  std::unordered_map<std::string, pxr::UsdGeomXformOp> xforms;
};
} // namespace scene
} // namespace dt