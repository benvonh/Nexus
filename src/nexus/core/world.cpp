#include "world.h"

#include "nexus/event/client.h"
#include "nexus/event/scene_reset_event.h"

#include "pxr/usd/usdGeom/cylinder.h"
#include "pxr/usd/usdGeom/metrics.h"
#include "pxr/usd/usdLux/domeLight.h"

void Nexus::World::NewStage(const std::string &path)
{
    LOG_EVENT("Creating new stage at '{}'", path);

    s_Mutex.lock();
    s_Stage = pxr::UsdStage::CreateNew(path);

    if (!pxr::UsdGeomSetStageUpAxis(s_Stage, pxr::UsdGeomTokens->z))
        LOG_ALERT("Stage could not set up axis to Z");

    if (!pxr::UsdGeomSetStageMetersPerUnit(s_Stage, 1.0))
        LOG_ALERT("Stage could not set meters per unit to 1.0");

    s_Mutex.unlock();

    Client::Send<SceneResetEvent>();
}

void Nexus::World::OpenStage(const std::string &path)
{
    LOG_EVENT("Opening stage at '{}'", path);

    s_Mutex.lock();
    s_Stage = pxr::UsdStage::Open(path);
    s_Mutex.unlock();

    Client::Send<SceneResetEvent>();
}

void Nexus::World::SaveStage()
{
    LOG_EVENT("Saving current stage...");

    s_Mutex.lock();
    s_Stage->SetFramesPerSecond(144.0);
    s_Stage->SetTimeCodesPerSecond(1.0);
    s_Stage->SetStartTimeCode(1.0);
    s_Stage->SetEndTimeCode(World::GetTime());
    s_Stage->Save();
    s_Mutex.unlock();

    Client::Send<SceneResetEvent>();
}

void Nexus::World::ExportStage(const std::string &path)
{
    LOG_EVENT("Exporting current stage to '{}'", path);

    s_Mutex.lock();
    s_Stage->SetFramesPerSecond(144.0);
    s_Stage->SetTimeCodesPerSecond(1.0);
    s_Stage->SetStartTimeCode(1.0);
    s_Stage->SetEndTimeCode(World::GetTime());
    s_Stage->Export(path);
    s_Mutex.unlock();

    Client::Send<SceneResetEvent>();
}

auto Nexus::World::CreateDefaultStage() -> pxr::UsdStageRefPtr
{
    auto stage = pxr::UsdStage::CreateInMemory();

    if (!pxr::UsdGeomSetStageUpAxis(stage, pxr::UsdGeomTokens->z))
        LOG_ALERT("Could not set stage up axis to Z");

    if (!pxr::UsdGeomSetStageMetersPerUnit(stage, 1.0))
        LOG_ALERT("Could not set stage meters per unit to 1.0");

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