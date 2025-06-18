#pragma once
#include "dt/scene/robot.hpp"
#include "pxr/usd/usd/stage.h"
#include <mutex>

namespace dt
{
  namespace scene
  {
    template <typename T>
    struct Permit
    {
      Permit(std::mutex &m, T &r) : _(m), ref(r) {}
      std::lock_guard<std::mutex> _;
      T &ref;
    };

    template <>
    struct Permit<Robot>
    {
      Permit(std::mutex &m, Robot &r) : _(m), robot(r) {}
      std::lock_guard<std::mutex> _;
      Robot &robot;
    };

    template <>
    struct Permit<pxr::UsdStageRefPtr>
    {
      Permit(std::mutex &m, pxr::UsdStageRefPtr &r) : _(m), stage(r) {}
      std::lock_guard<std::mutex> _;
      pxr::UsdStageRefPtr &stage;
    };

    using RobotPermit = Permit<Robot>;
    using StagePermit = Permit<pxr::UsdStageRefPtr>;
  }
}