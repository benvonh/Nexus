#pragma once
#include "pxr/usd/usd/stage.h"
#include "pxr/usd/usdGeom/xformOp.h"
#include <unordered_map>
#include <string>

namespace dt
{
  namespace scene
  {
    // TODO: Properly - multiple robots?
    struct Robot
    {
      std::unordered_map<std::string, pxr::UsdGeomXformOp> xforms;

      Robot() = default;
      ~Robot() = default;
      Robot(Robot &&) = delete;
      Robot(const Robot &) = delete;
      Robot &operator=(Robot &&) = delete;
      Robot &operator=(const Robot &) = delete;

      void FromURDF(pxr::UsdStageRefPtr stage, const std::string &path);
    };
  }
}