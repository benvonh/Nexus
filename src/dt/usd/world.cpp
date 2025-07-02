#include "world.h"
#include "dt/logging.h"
#include "pxr/usd/usdGeom/metrics.h"
#include "pxr/usd/usdGeom/cylinder.h"
#include "pxr/usd/usdLux/domeLight.h"

pxr::UsdStageRefPtr dt::World::default_stage()
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