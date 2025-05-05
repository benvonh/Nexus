#pragma once
#include "pxr/usd/usdGeom/xformOp.h"
#include <unordered_map>
#include <string>

namespace dt
{
  namespace scene
  {
    struct Robot
    {
      Robot();
      ~Robot() = default;
      Robot(Robot &&) = delete;
      Robot(const Robot &) = delete;
      Robot &operator=(Robot &&) = delete;
      Robot &operator=(const Robot &) = delete;

      void ParseURDF(const std::string &path);

      std::unordered_map<std::string, pxr::UsdGeomXformOp> xforms;
    };
  }
}