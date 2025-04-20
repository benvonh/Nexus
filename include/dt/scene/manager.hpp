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
#include "dt/scene/view.hpp"

#include "pxr/usd/usd/stage.h"
#include "pxr/usd/usdGeom/camera.h"
#include "pxr/usd/usdGeom/sphere.h"
#include "pxr/usd/usdGeom/xformOp.h"
#include "pxr/usd/usdGeom/cylinder.h"
#include "pxr/usd/usdLux/domeLight.h"
#include "pxr/usd/sdf/timeCode.h"

#include <mutex>
#include <chrono>
#include <iostream>
#include <stacktrace>
#include <filesystem>

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

      friend class View;

    public:
      Manager() = delete;
      ~Manager() = delete;
      Manager(Manager &&) = delete;
      Manager(const Manager &) = delete;
      Manager &operator=(Manager &&) = delete;
      Manager &operator=(const Manager &) = delete;

      static bool OpenFile(const std::filesystem::path &path)
      {
        try
        {
          // auto stagePtr = pxr::UsdStage::Open(path.string());
          auto stagePtr = pxr::UsdStage::CreateInMemory("temp.usd");

          if (stagePtr == nullptr)
            return false;

          s::stage = stagePtr;

          s::view.camera = pxr::UsdGeomCamera::Define(stagePtr, pxr::SdfPath("/Camera"));
          s::view.camera.CreateProjectionAttr(pxr::VtValue(pxr::TfToken("perspective")));
          s::view.camera.CreateFocalLengthAttr(pxr::VtValue(18.f));
          s::view.camera.CreateFStopAttr(pxr::VtValue(8.f));
          s::view.translate = s::view.camera.AddTranslateOp();
          s::view.rotateY = s::view.camera.AddRotateYOp();
          s::view.rotateX = s::view.camera.AddRotateXOp();

          s::view.translate.Set(pxr::GfVec3d(0.0, 0.0, 10.0));
          s::view.rotateX.Set(0.f);
          s::view.rotateY.Set(0.f);

          auto dome = pxr::UsdLuxDomeLight::Define(stagePtr, pxr::SdfPath("/Dome"));
          dome.CreateTextureFileAttr(pxr::VtValue(pxr::SdfAssetPath("studio_small_09_4k.exr")));
          // dome.CreateGuideRadiusAttr().Set(100000000.f);
          dome.CreateIntensityAttr().Set(1.f);
          dome.CreateExposureAttr().Set(1.f);
          dome.CreateTextureFormatAttr().Set(pxr::TfToken("latlong"));

          // TODO: Sphere is not necessary for seeing environment map
          // auto c = pxr::UsdGeomSphere::Define(stagePtr, pxr::SdfPath("/Sphere"));
          // c.CreateRadiusAttr().Set(100000.0);

          // TODO: Temporary
          pxr::UsdGeomCylinder axisX = pxr::UsdGeomCylinder::Define(stagePtr, pxr::SdfPath("/Xaxis"));
          pxr::UsdGeomCylinder axisY = pxr::UsdGeomCylinder::Define(stagePtr, pxr::SdfPath("/Yaxis"));
          pxr::UsdGeomCylinder axisZ = pxr::UsdGeomCylinder::Define(stagePtr, pxr::SdfPath("/Zaxis"));
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

          pxr::UsdGeomCylinder axisX1 = pxr::UsdGeomCylinder::Define(stagePtr, pxr::SdfPath("/Xaxis1"));
          pxr::UsdGeomCylinder axisY1 = pxr::UsdGeomCylinder::Define(stagePtr, pxr::SdfPath("/Yaxis1"));
          pxr::UsdGeomCylinder axisZ1 = pxr::UsdGeomCylinder::Define(stagePtr, pxr::SdfPath("/Zaxis1"));
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
        catch (const std::exception &e)
        {
          std::cerr << "\nERROR: " << e.what() << '\n';
          std::cout << std::stacktrace::current() << '\n';
          return false;
        }
        return true;
      }

      static void SaveFile()
      {
        if (s::stage == nullptr)
          return;

        try
        {
          s::stage->SetFramesPerSecond(144.0);
          s::stage->SetTimeCodesPerSecond(1.0);
          s::stage->SetStartTimeCode(1.0);
          s::stage->SetEndTimeCode(s::GetTimeCode().GetValue());
          s::stage->GetRootLayer()->Export("temp.usdc");
        }
        catch (const std::exception &e)
        {
          std::cerr << "\nERROR: " << e.what() << '\n';
          std::cout << std::stacktrace::current() << '\n';
        }
      }

      /// @brief Return a pointer to View. The "raw" in this name has a double
      /// meaning which are that the pointer is raw (though not heap-allocated)
      /// and View is not returned in a Loan with a mutex. Thus, View
      /// bares the responsibility of ensuring thread safety.
      /// @return Pointer to View
      static View *GetRawView()
      {
        return &s::view;
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
      static inline View view;
      static inline std::mutex mutex;
      static inline pxr::UsdStageRefPtr stage;
    };
  }
}