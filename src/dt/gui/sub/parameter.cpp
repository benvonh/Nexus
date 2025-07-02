#include "parameter.h"
#include "dt/usd/world.h"
#include "imgui.h"

static constexpr const char *DRAW_MODES[] = {
    "Points",
    "Wireframe",
    "Wireframe on Surface",
    "Shaded Flat",
    "Shaded Smooth",
    "Geometry Only",
    "Geometry Flat",
    "Geometry Smooth"};

static constexpr const char *CULL_STYLES[] = {
    "No Opinion",
    "Nothing",
    "Back",
    "Front",
    "Back Unless Double-Sided"};

/*============================================================================*/
dt::Parameter::Parameter()
{
    _Params.enableLighting = true;
    _Params.enableSceneLights = true;
    _Params.enableSceneMaterials = true;
    _Params.showGuides = false;
    _Params.showProxy = false;
    _Params.showRender = false;
    _Params.forceRefresh = false;
    _Params.enableSampleAlphaToCoverage = true;
    _Params.gammaCorrectColors = false;
    _Params.clearColor = pxr::GfVec4f(0.5f, 0.5f, 0.5f, 1.f);
    _Params.complexity = 1.f;
    _Params.drawMode = pxr::UsdImagingGLDrawMode::DRAW_SHADED_SMOOTH;
    _Params.cullStyle = pxr::UsdImagingGLCullStyle::CULL_STYLE_BACK_UNLESS_DOUBLE_SIDED;
    _Params.frame = 0;
}

/*============================================================================*/
void dt::Parameter::draw()
{
    ImGui::Checkbox("Lighting", &_Params.enableLighting);
    ImGui::Checkbox("Scene Lights", &_Params.enableSceneLights);
    ImGui::Checkbox("Scene Materials", &_Params.enableSceneMaterials);
    ImGui::Checkbox("Show Guides", &_Params.showGuides);
    ImGui::Checkbox("Show Proxy", &_Params.showProxy);
    ImGui::Checkbox("Show Render", &_Params.showRender);
    ImGui::Checkbox("Force Refresh", &_Params.forceRefresh);
    ImGui::Checkbox("Sample Alpha to Coverage", &_Params.enableSampleAlphaToCoverage);
    ImGui::Checkbox("Gamma Correct Colors", &_Params.gammaCorrectColors);
    ImGui::ColorEdit4("Clear Color", &_Params.clearColor[0]);
    ImGui::SliderFloat("Complexity", &_Params.complexity, 1.f, 1.5f, "%.1f");
    ImGui::Combo("Draw Mode", (int *)&_Params.drawMode, DRAW_MODES, std::size(DRAW_MODES));
    ImGui::Combo("Cull Style", (int *)&_Params.cullStyle, CULL_STYLES, std::size(CULL_STYLES));
    ImGui::Checkbox("Live Playback", &_Live);

    if (_Live)
    {
        _Time = _Latest;
        _Latest = World::get_time();
    }
    if (ImGui::SliderFloat("Time Code", &_Time, 0.f, _Latest))
    {
        _Live = false;
    }
    _Params.frame = _Time;
}