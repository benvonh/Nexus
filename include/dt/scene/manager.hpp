/**
 * @file manager.hpp
 * @author Benjamin von Snarski (@benvonh)
 * @brief Header for the scene manager class.
 * @details This class is designed to have purely static memebers, effectively
 * a singleton in global space.
 * @version 0.1
 * @date 2025-04-18
 *
 * @copyright Copyright (c) 2025
 *
 */
#pragma once
#include "dt/scene/loan.hpp"
#include "pxr/usd/usd/stage.h"
#include "pxr/usd/usdGeom/camera.h"
#include "pxr/usd/usdGeom/sphere.h"
#include "pxr/usd/usdGeom/xformOp.h"
#include "pxr/usd/usdGeom/cylinder.h"
#include "pxr/usd/usdLux/domeLight.h"
#include "pxr/usd/sdf/timeCode.h"
#include <string>
#include <chrono>
#include <mutex>

namespace dt
{
  namespace scene
  {
    class Manager final
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

      static void NewStage(const std::string &path)
      {
        std::lock_guard<std::mutex> guard(s::mutex);
        s::stage = pxr::UsdStage::CreateNew(path);
        s::DebugStage();
      }

      static void OpenStage(const std::string &path)
      {
        std::lock_guard<std::mutex> guard(s::mutex);
        s::stage = pxr::UsdStage::Open(path);
      }

      static void DebugStage()
      {
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

        // TODO: Sphere is not necessary for seeing environment map
        // auto c = pxr::UsdGeomSphere::Define(stagePtr, pxr::SdfPath("/Sphere"));
        // c.CreateRadiusAttr().Set(100000.0);

        // TODO: Temporary
        pxr::UsdGeomCylinder axisX = pxr::UsdGeomCylinder::Define(s::stage, pxr::SdfPath("/Xaxis"));
        pxr::UsdGeomCylinder axisY = pxr::UsdGeomCylinder::Define(s::stage, pxr::SdfPath("/Yaxis"));
        pxr::UsdGeomCylinder axisZ = pxr::UsdGeomCylinder::Define(s::stage, pxr::SdfPath("/Zaxis"));
        axisX.CreateAxisAttr(pxr::VtValue(pxr::TfToken("X")));
        axisY.CreateAxisAttr(pxr::VtValue(pxr::TfToken("Y")));
        axisZ.CreateAxisAttr(pxr::VtValue(pxr::TfToken("Z")));
        axisX.CreateHeightAttr(pxr::VtValue(1.0));
        axisY.CreateHeightAttr(pxr::VtValue(1.0));
        axisZ.CreateHeightAttr(pxr::VtValue(1.0));
        axisX.CreateRadiusAttr(pxr::VtValue(0.1));
        axisY.CreateRadiusAttr(pxr::VtValue(0.1));
        axisZ.CreateRadiusAttr(pxr::VtValue(0.1));
        axisX.AddTranslateXOp().Set(0.5);
        axisY.AddTranslateYOp().Set(0.5);
        axisZ.AddTranslateZOp().Set(0.5);
        axisX.CreateDisplayColorPrimvar().Set(pxr::VtArray(pxr::GfVec3f(1, 0, 0)));
        axisY.CreateDisplayColorPrimvar().Set(pxr::VtArray(pxr::GfVec3f(0, 1, 0)));
        axisZ.CreateDisplayColorPrimvar().Set(pxr::VtArray(pxr::GfVec3f(0, 0, 1)));

        pxr::UsdGeomCylinder axisX1 = pxr::UsdGeomCylinder::Define(s::stage, pxr::SdfPath("/Xaxis1"));
        pxr::UsdGeomCylinder axisY1 = pxr::UsdGeomCylinder::Define(s::stage, pxr::SdfPath("/Yaxis1"));
        pxr::UsdGeomCylinder axisZ1 = pxr::UsdGeomCylinder::Define(s::stage, pxr::SdfPath("/Zaxis1"));
        axisX1.CreateAxisAttr(pxr::VtValue(pxr::TfToken("X")));
        axisY1.CreateAxisAttr(pxr::VtValue(pxr::TfToken("Y")));
        axisZ1.CreateAxisAttr(pxr::VtValue(pxr::TfToken("Z")));
        axisX1.CreateHeightAttr(pxr::VtValue(1.0));
        axisY1.CreateHeightAttr(pxr::VtValue(1.0));
        axisZ1.CreateHeightAttr(pxr::VtValue(1.0));
        axisX1.CreateRadiusAttr(pxr::VtValue(0.1));
        axisY1.CreateRadiusAttr(pxr::VtValue(0.1));
        axisZ1.CreateRadiusAttr(pxr::VtValue(0.1));
        axisX1.AddTranslateXOp().Set(10.5);
        axisY1.AddTranslateXOp().Set(10.0);
        axisY1.AddTranslateYOp().Set(0.5);
        axisZ1.AddTranslateXOp().Set(10.0);
        axisZ1.AddTranslateZOp().Set(0.5);
        axisX1.CreateDisplayColorPrimvar().Set(pxr::VtArray(pxr::GfVec3f(1, 0, 0)));
        axisY1.CreateDisplayColorPrimvar().Set(pxr::VtArray(pxr::GfVec3f(0, 1, 0)));
        axisZ1.CreateDisplayColorPrimvar().Set(pxr::VtArray(pxr::GfVec3f(0, 0, 1)));
      }

      static void SaveStage()
      {
        std::lock_guard<std::mutex> guard(s::mutex);
        s::stage->SetFramesPerSecond(144.0);
        s::stage->SetTimeCodesPerSecond(1.0);
        s::stage->SetStartTimeCode(1.0);
        s::stage->SetEndTimeCode(s::GetTimeCode().GetValue());
        s::stage->Save();
      }

      static void ExportStage(const std::string &path)
      {
        std::lock_guard<std::mutex> guard(s::mutex);
        s::stage->SetFramesPerSecond(144.0);
        s::stage->SetTimeCodesPerSecond(1.0);
        s::stage->SetStartTimeCode(1.0);
        s::stage->SetEndTimeCode(s::GetTimeCode().GetValue());
        s::stage->Export(path);
      }

      static std::lock_guard<std::mutex> GetLock()
      {
        return std::lock_guard<std::mutex>(s::mutex);
      }

      static pxr::UsdStageRefPtr GetStageRaw()
      {
        return s::stage;
      }

      static Loan<pxr::UsdStageRefPtr> GetStageLoan()
      {
        return {s::mutex, s::stage};
      }

      static pxr::UsdTimeCode GetTimeCode()
      {
        const auto duration = std::chrono::steady_clock::now() - START;
        const auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(duration);
        return static_cast<double>(nanoseconds.count()) * 1e-9;
      }

      static inline const auto START = std::chrono::steady_clock::now();

    private:
      static inline std::mutex mutex;
      // static inline pxr::UsdStageRefPtr stage = pxr::UsdStage::CreateInMemory();
      static inline pxr::UsdStageRefPtr stage = pxr::UsdStage::Open("temp.usdc");
    };
  }
}