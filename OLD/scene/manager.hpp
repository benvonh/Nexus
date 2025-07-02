#pragma once
#include "dt/util.hpp"
#include "dt/scene/permit.hpp"
#include "dt/scene/robot.hpp"
#include "pxr/usd/sdf/timeCode.h"
#include "pxr/usd/usd/stage.h"
#include "pxr/usd/usdGeom/camera.h"
#include "pxr/usd/usdGeom/cylinder.h"
#include "pxr/usd/usdGeom/metrics.h"
#include "pxr/usd/usdGeom/sphere.h"
#include "pxr/usd/usdGeom/xformOp.h"
#include "pxr/usd/usdLux/domeLight.h"
#include <chrono>
#include <iostream>
#include <mutex>
#include <string>

namespace dt
{
  namespace scene
  {
    ///
    /// @brief Create a default stage in memory
    /// @return Default USD stage
    ///
    auto DefaultStage() -> pxr::default_stage
    {
      pxr::default_stage stage = pxr::UsdStage::CreateInMemory();

      if (!pxr::UsdGeomSetStageUpAxis(stage, pxr::UsdGeomTokens->z))
      {
        log::alert("Could not set stage up axis to Z");
      }

      if (!pxr::UsdGeomSetStageMetersPerUnit(stage, 1.0))
      {
        log::alert("Could not set stage meters per unit to 1.0");
      }

      auto dome = pxr::UsdLuxDomeLight::Define(stage, pxr::SdfPath("/studio_dome"));
      auto asset = pxr::SdfAssetPath("assets/studio_small_09_4k.exr");
      dome.CreateTextureFileAttr(pxr::VtValue(asset));
      dome.OrientToStageUpAxis();

      auto axisX = pxr::UsdGeomCylinder::Define(stage, pxr::SdfPath("/axis/x"));
      auto axisY = pxr::UsdGeomCylinder::Define(stage, pxr::SdfPath("/axis/y"));
      auto axisZ = pxr::UsdGeomCylinder::Define(stage, pxr::SdfPath("/axis/z"));
      axisX.CreateAxisAttr(pxr::VtValue(pxr::UsdGeomTokens->x));
      axisY.CreateAxisAttr(pxr::VtValue(pxr::UsdGeomTokens->y));
      axisZ.CreateAxisAttr(pxr::VtValue(pxr::UsdGeomTokens->z));
      axisX.CreateHeightAttr(pxr::VtValue(1.0));
      axisY.CreateHeightAttr(pxr::VtValue(1.0));
      axisZ.CreateHeightAttr(pxr::VtValue(1.0));
      axisX.CreateRadiusAttr(pxr::VtValue(0.001));
      axisY.CreateRadiusAttr(pxr::VtValue(0.001));
      axisZ.CreateRadiusAttr(pxr::VtValue(0.001));
      axisX.AddTranslateXOp().Set(0.5);
      axisY.AddTranslateYOp().Set(0.5);
      axisZ.AddTranslateZOp().Set(0.5);
      axisX.CreateDisplayColorPrimvar().Set(pxr::VtArray<pxr::GfVec3f>({pxr::GfVec3f(1.0, 0.0, 0.0)}));
      axisY.CreateDisplayColorPrimvar().Set(pxr::VtArray<pxr::GfVec3f>({pxr::GfVec3f(0.0, 1.0, 0.0)}));
      axisZ.CreateDisplayColorPrimvar().Set(pxr::VtArray<pxr::GfVec3f>({pxr::GfVec3f(0.0, 0.0, 1.0)}));
      log::debug("Created default stage!");
      return stage;
    }

    enum class Action
    {
      NEW,
      OPEN,
      SAVE,
      EXPORT
    };

    class Manager
    {
      /// @brief An alias to the class itManager which has two meanings:
      /// 1) [s]elf as it is alised to the class itManager; and
      /// 2) [s]tatic as it is a purely static class.
      // using s = Manager;

    public:
      Manager() = delete;
      ~Manager() = delete;
      Manager(Manager &&) = delete;
      Manager(const Manager &) = delete;
      Manager &operator=(Manager &&) = delete;
      Manager &operator=(const Manager &) = delete;

      static double GetTime()
      {
        const auto dur = std::chrono::steady_clock::now() - START;
        const auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(dur);
        return ns.count() * 1e-9;
      }

      static Permit<pxr::default_stage> GetStagePermit()
      {
        return Permit(Manager::mutex, Manager::stage);
      }

      static Permit<Robot> GetRobotPermit()
      {
        return Permit(Manager::mutex, Manager::robot);
      }

      static void SetRobot(const std::string &path)
      {
        std::lock_guard _(Manager::mutex);
        Manager::robot.FromURDF(Manager::stage, path);
      }

      template <Action A>
      static void HandleStage(const std::string &path = "")
      {
        std::lock_guard _(Manager::mutex);

        if constexpr (A == Action::NEW)
        {
          log::event("Creating new stage at {}", path);
          Manager::stage = pxr::UsdStage::CreateNew(path);
        }
        if constexpr (A == Action::OPEN)
        {
          log::event("Opening stage at {}", path);
          Manager::stage = pxr::UsdStage::Open(path);
        }
        if constexpr (A == Action::SAVE || A == Action::EXPORT)
        {
          Manager::stage->SetFramesPerSecond(144.0);
          Manager::stage->SetTimeCodesPerSecond(1.0);
          Manager::stage->SetStartTimeCode(1.0);
          Manager::stage->SetEndTimeCode(Manager::GetTime());

          if constexpr (A == Action::SAVE)
          {
            const auto id = Manager::stage->GetRootLayer()->GetIdentifier();
            log::event("Saving stage to {}", id);
            Manager::stage->Save();
          }
          else
          {
            log::event("Exporting stage to {}", path);
            Manager::stage->Export(path);
          }
        }
      }

    private:
      static inline Robot robot;
      static inline std::mutex mutex;
      static inline pxr::default_stage stage = scene::DefaultStage();
      static inline const auto START = std::chrono::steady_clock::now();
    };
  }
}