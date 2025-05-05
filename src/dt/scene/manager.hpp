#pragma once
#include "dt/base.hpp"
#include "dt/scene/robot.hpp"
#include "pxr/usd/sdf/timeCode.h"
#include "pxr/usd/usd/stage.h"
#include "pxr/usd/usdGeom/camera.h"
#include "pxr/usd/usdGeom/cylinder.h"
#include "pxr/usd/usdGeom/metrics.h"
#include "pxr/usd/usdGeom/sphere.h"
#include "pxr/usd/usdGeom/xformOp.h"
#include "pxr/usd/usdLux/domeLight.h"
#include "pxr/base/vt/array.h"
#include <chrono>
#include <iostream>
#include <mutex>
#include <string>

namespace dt
{
  namespace scene
  {
    enum class Action
    {
      NEW,
      OPEN,
      SAVE,
      EXPORT
    };

    template <typename T>
    struct Permit
    {
      Permit(std::mutex &m, T &r) : guard(m), it(r) {}
      std::lock_guard<std::mutex> guard;
      T &it;
    };

    template <>
    struct Permit<pxr::UsdStageRefPtr>
    {
      Permit(std::mutex &m, pxr::UsdStageRefPtr &r) : guard(m), stage(r) {}
      std::lock_guard<std::mutex> guard;
      pxr::UsdStageRefPtr &stage;
    };

    template <>
    struct Permit<Robot>
    {
      Permit(std::mutex &m, Robot &r) : guard(m), robot(r) {}
      std::lock_guard<std::mutex> guard;
      Robot &robot;
    };

    using StagePermit = Permit<pxr::UsdStageRefPtr>;
    using RobotPermit = Permit<Robot>;

    class Manager
    {
      /// @brief An alias to the class itself which has two meanings:
      /// 1) [s]elf as it is alised to the class itself; and
      /// 2) [s]tatic as it is a purely static class.
      using s = Manager;

    public:
      Manager() = delete;
      ~Manager() = delete;
      Manager(Manager &&) = delete;
      Manager(const Manager &) = delete;
      Manager &operator=(Manager &&) = delete;
      Manager &operator=(const Manager &) = delete;

      template <Action A>
      static void HandleStage(const std::string &path = "")
      {
        std::lock_guard _(s::mutex);

        if constexpr (A == Action::NEW)
        {
          log::event("Creating new stage at {}", path);
          s::stage = pxr::UsdStage::CreateNew(path);
        }
        if constexpr (A == Action::OPEN)
        {
          log::event("Opening stage at {}", path);
          s::stage = pxr::UsdStage::Open(path);
        }
        if constexpr (A == Action::SAVE || A == Action::EXPORT)
        {
          s::stage->SetFramesPerSecond(144.0);
          s::stage->SetTimeCodesPerSecond(1.0);
          s::stage->SetStartTimeCode(1.0);
          s::stage->SetEndTimeCode(s::GetTime());

          if constexpr (A == Action::SAVE)
          {
            const auto id = s::stage->GetRootLayer()->GetIdentifier();
            log::event("Saving stage to {}", id);
            s::stage->Save();
          }
          else
          {
            log::event("Exporting stage to {}", path);
            s::stage->Export(path);
          }
        }
      }

      static void DebugStage()
      {
        if (!pxr::UsdGeomSetStageUpAxis(s::stage, pxr::UsdGeomTokens->z))
          throw std::runtime_error("wtf no z??");

        auto camera = pxr::UsdGeomCamera::Define(s::stage, pxr::SdfPath("/RandomCamera"));
        camera.CreateProjectionAttr(pxr::VtValue(pxr::TfToken("perspective")));
        camera.CreateFocalLengthAttr(pxr::VtValue(18.f));
        camera.CreateFStopAttr(pxr::VtValue(8.f));
        auto translate = camera.AddTranslateOp();
        auto rotateY = camera.AddRotateYOp();
        auto rotateX = camera.AddRotateXOp();

        translate.Set(pxr::GfVec3d(0.0, 0.0, 10.0));
        rotateX.Set(0.f);
        rotateY.Set(0.f);

        auto dome = pxr::UsdLuxDomeLight::Define(s::stage, pxr::SdfPath("/Dome"));
        dome.CreateTextureFileAttr(pxr::VtValue(pxr::SdfAssetPath("studio_small_09_4k.exr")));
        // dome.CreateGuideRadiusAttr().Set(100000000.f);
        dome.CreateIntensityAttr().Set(1.f);
        dome.CreateExposureAttr().Set(1.f);
        dome.CreateTextureFormatAttr().Set(pxr::TfToken("latlong"));
        dome.OrientToStageUpAxis();

        pxr::UsdGeomCylinder axisX = pxr::UsdGeomCylinder::Define(s::stage, pxr::SdfPath("/Xaxis"));
        pxr::UsdGeomCylinder axisY = pxr::UsdGeomCylinder::Define(s::stage, pxr::SdfPath("/Yaxis"));
        pxr::UsdGeomCylinder axisZ = pxr::UsdGeomCylinder::Define(s::stage, pxr::SdfPath("/Zaxis"));
        axisX.CreateAxisAttr(pxr::VtValue(pxr::TfToken("X")));
        axisY.CreateAxisAttr(pxr::VtValue(pxr::TfToken("Y")));
        axisZ.CreateAxisAttr(pxr::VtValue(pxr::TfToken("Z")));
        axisX.CreateHeightAttr(pxr::VtValue(0.6));
        axisY.CreateHeightAttr(pxr::VtValue(0.6));
        axisZ.CreateHeightAttr(pxr::VtValue(0.6));
        axisX.CreateRadiusAttr(pxr::VtValue(0.01));
        axisY.CreateRadiusAttr(pxr::VtValue(0.01));
        axisZ.CreateRadiusAttr(pxr::VtValue(0.01));
        axisX.AddTranslateXOp().Set(0.3);
        axisY.AddTranslateYOp().Set(0.3);
        axisZ.AddTranslateZOp().Set(0.3);
        axisX.CreateDisplayColorPrimvar().Set(pxr::VtArray<pxr::GfVec3f>({pxr::GfVec3f(1.0, 0.0, 0.0)}));
        axisY.CreateDisplayColorPrimvar().Set(pxr::VtArray<pxr::GfVec3f>({pxr::GfVec3f(0.0, 1.0, 0.0)}));
        axisZ.CreateDisplayColorPrimvar().Set(pxr::VtArray<pxr::GfVec3f>({pxr::GfVec3f(0.0, 0.0, 1.0)}));

        pxr::UsdGeomCylinder axisX1 = pxr::UsdGeomCylinder::Define(s::stage, pxr::SdfPath("/Xaxis1"));
        pxr::UsdGeomCylinder axisY1 = pxr::UsdGeomCylinder::Define(s::stage, pxr::SdfPath("/Yaxis1"));
        pxr::UsdGeomCylinder axisZ1 = pxr::UsdGeomCylinder::Define(s::stage, pxr::SdfPath("/Zaxis1"));
        axisX1.CreateAxisAttr(pxr::VtValue(pxr::TfToken("X")));
        axisY1.CreateAxisAttr(pxr::VtValue(pxr::TfToken("Y")));
        axisZ1.CreateAxisAttr(pxr::VtValue(pxr::TfToken("Z")));
        axisX1.CreateHeightAttr(pxr::VtValue(0.6));
        axisY1.CreateHeightAttr(pxr::VtValue(0.6));
        axisZ1.CreateHeightAttr(pxr::VtValue(0.6));
        axisX1.CreateRadiusAttr(pxr::VtValue(0.01));
        axisY1.CreateRadiusAttr(pxr::VtValue(0.01));
        axisZ1.CreateRadiusAttr(pxr::VtValue(0.01));
        axisX1.AddTranslateXOp().Set(10.3);
        axisY1.AddTranslateXOp().Set(10.0);
        axisY1.AddTranslateYOp().Set(0.3);
        axisZ1.AddTranslateXOp().Set(10.0);
        axisZ1.AddTranslateZOp().Set(0.3);
        axisX1.CreateDisplayColorPrimvar().Set(pxr::VtArray<pxr::GfVec3f>({pxr::GfVec3f(1.0, 0.0, 0.0)}));
        axisY1.CreateDisplayColorPrimvar().Set(pxr::VtArray<pxr::GfVec3f>({pxr::GfVec3f(0.0, 1.0, 0.0)}));
        axisZ1.CreateDisplayColorPrimvar().Set(pxr::VtArray<pxr::GfVec3f>({pxr::GfVec3f(0.0, 0.0, 1.0)}));
      }

      static void SetRobot(const std::string &path = "r2d2.urdf.xml")
      {
        s::robot.ParseURDF(path);
      }

      static std::lock_guard<std::mutex> GetLockGuard()
      {
        return std::lock_guard<std::mutex>(s::mutex);
      }

      static Permit<pxr::UsdStageRefPtr> GetStagePermit()
      {
        return Permit(s::mutex, s::stage);
      }

      static Permit<Robot> GetRobotPermit()
      {
        return Permit(s::mutex, s::robot);
      }

      static double GetTime()
      {
        const auto dur = std::chrono::steady_clock::now() - START;
        const auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(dur);
        return ns.count() * 1e-9;
      }

    private:
      static inline Robot robot;
      static inline std::mutex mutex;
      static inline pxr::UsdStageRefPtr stage = pxr::UsdStage::CreateInMemory();

      static inline const auto START = std::chrono::steady_clock::now();
    };
  }
}