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
    struct Permit<pxr::default_stage>
    {
      Permit(std::mutex &m, pxr::default_stage &r) : _(m), stage(r) {}
      std::lock_guard<std::mutex> _;
      pxr::default_stage &stage;
    };

    using RobotPermit = Permit<Robot>;
    using StagePermit = Permit<pxr::default_stage>;
  }
}