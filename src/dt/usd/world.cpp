#include "world.h"

#include "dt/logging.h"

#include "pxr/usd/usdGeom/cylinder.h"
#include "pxr/usd/usdGeom/metrics.h"
#include "pxr/usd/usdLux/domeLight.h"

/*============================================================================*/
void dt::World::NewStage(const std::string &path)
{
    log::event("Creating a new stage at '{}'", path);
    std::lock_guard guard(__Mutex);
    __Stage = pxr::UsdStage::CreateNew(path);
}

/*============================================================================*/
void dt::World::OpenStage(const std::string &path)
{
    log::event("Opening a stage at '{}'", path);
    std::lock_guard guard(__Mutex);
    __Stage = pxr::UsdStage::Open(path);
}

/*============================================================================*/
void dt::World::SaveStage()
{
    log::event("Saving current stage");
    std::lock_guard guard(__Mutex);
    __Stage->SetFramesPerSecond(144.0);
    __Stage->SetTimeCodesPerSecond(1.0);
    __Stage->SetStartTimeCode(1.0);
    __Stage->SetEndTimeCode(GetTime());
    __Stage->Save();
}

/*============================================================================*/
void dt::World::ExportStage(const std::string &path)
{
    log::event("Exporting current stage to '{}'", path);
    std::lock_guard guard(__Mutex);
    __Stage->SetFramesPerSecond(144.0);
    __Stage->SetTimeCodesPerSecond(1.0);
    __Stage->SetStartTimeCode(1.0);
    __Stage->SetEndTimeCode(GetTime());
    __Stage->Export(path);
}

/*============================================================================*/
pxr::UsdStageRefPtr dt::World::__DefaultStage()
{
    auto stage = pxr::UsdStage::CreateInMemory();

    if (!pxr::UsdGeomSetStageUpAxis(stage, pxr::UsdGeomTokens->z))
    {
        log::alert("Could not set stage up axis to Z");
    }

    if (!pxr::UsdGeomSetStageMetersPerUnit(stage, 1.0))
    {
        log::alert("Could not set stage meters per unit to 1.0");
    }

    auto dome = pxr::UsdLuxDomeLight::Define(stage, pxr::SdfPath("/studio_dome"));
    auto asset = pxr::SdfAssetPath(R"(C:\pixi_ws\DigitalTwin\assets\studio_small_09_4k.exr)");
    dome.CreateTextureFileAttr(pxr::VtValue(asset));
    dome.OrientToStageUpAxis();

    auto axisX = pxr::UsdGeomCylinder::Define(stage, pxr::SdfPath("/axis/x"));
    auto axisY = pxr::UsdGeomCylinder::Define(stage, pxr::SdfPath("/axis/y"));
    auto axisZ = pxr::UsdGeomCylinder::Define(stage, pxr::SdfPath("/axis/z"));
    axisX.CreateAxisAttr(pxr::VtValue(pxr::UsdGeomTokens->x));
    axisY.CreateAxisAttr(pxr::VtValue(pxr::UsdGeomTokens->y));
    axisZ.CreateAxisAttr(pxr::VtValue(pxr::UsdGeomTokens->z));
    axisX.CreateHeightAttr(pxr::VtValue(0.5));
    axisY.CreateHeightAttr(pxr::VtValue(0.5));
    axisZ.CreateHeightAttr(pxr::VtValue(0.5));
    axisX.CreateRadiusAttr(pxr::VtValue(0.01));
    axisY.CreateRadiusAttr(pxr::VtValue(0.01));
    axisZ.CreateRadiusAttr(pxr::VtValue(0.01));
    axisX.AddTranslateXOp().Set(0.25);
    axisY.AddTranslateYOp().Set(0.25);
    axisZ.AddTranslateZOp().Set(0.25);
    axisX.CreateDisplayColorPrimvar().Set(pxr::VtArray<pxr::GfVec3f>({pxr::GfVec3f(1.0, 0.0, 0.0)}));
    axisY.CreateDisplayColorPrimvar().Set(pxr::VtArray<pxr::GfVec3f>({pxr::GfVec3f(0.0, 1.0, 0.0)}));
    axisZ.CreateDisplayColorPrimvar().Set(pxr::VtArray<pxr::GfVec3f>({pxr::GfVec3f(0.0, 0.0, 1.0)}));
    return stage;
}