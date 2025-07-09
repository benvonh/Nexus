#include "parameter.h"

#include "dt/core/world.h"

#include "imgui.h"

static const char *DRAW_MODES[] = {
    "Points",
    "Wireframe",
    "Wireframe on Surface",
    "Shaded Flat",
    "Shaded Smooth",
    "Geometry Only",
    "Geometry Flat",
    "Geometry Smooth"};

static const char *CULL_STYLES[] = {
    "No Opinion",
    "Nothing",
    "Back",
    "Front",
    "Back Unless Double-Sided"};

dt::Parameter::Parameter()
{
    _Params.clearColor = pxr::GfVec4f(0.5f, 0.5f, 0.5f, 1.f);
    _Params.complexity = 1.f;
    _Params.cullStyle = pxr::UsdImagingGLCullStyle::CULL_STYLE_BACK_UNLESS_DOUBLE_SIDED;
    _Params.drawMode = pxr::UsdImagingGLDrawMode::DRAW_SHADED_SMOOTH;
    _Params.enableLighting = true;
    _Params.enableSampleAlphaToCoverage = true;
    _Params.enableSceneLights = true;
    _Params.enableSceneMaterials = true;
    _Params.forceRefresh = false;
    _Params.frame = 0;
    _Params.gammaCorrectColors = false;
    _Params.showGuides = false;
    _Params.showProxy = false;
    _Params.showRender = false;
    __Params = _Params;
}

void dt::Parameter::Draw()
{
    if (ImGui::Begin("Parameter"))
    {
        ImGui::Checkbox("Lighting", &__Params.enableLighting);
        ImGui::Checkbox("Scene Lights", &__Params.enableSceneLights);
        ImGui::Checkbox("Scene Materials", &__Params.enableSceneMaterials);
        ImGui::Checkbox("Show Guides", &__Params.showGuides);
        ImGui::Checkbox("Show Proxy", &__Params.showProxy);
        ImGui::Checkbox("Show Render", &__Params.showRender);
        ImGui::Checkbox("Force Refresh", &__Params.forceRefresh);
        ImGui::Checkbox("Sample Alpha to Coverage", &__Params.enableSampleAlphaToCoverage);
        ImGui::Checkbox("Gamma Correct Colors", &__Params.gammaCorrectColors);
        ImGui::ColorEdit4("Clear Color", &__Params.clearColor[0]);
        ImGui::SliderFloat("Complexity", &__Params.complexity, 1.f, 1.5f, "%.1f");
        ImGui::Combo("Draw Mode", (int *)&__Params.drawMode, DRAW_MODES, std::size(DRAW_MODES));
        ImGui::Combo("Cull Style", (int *)&__Params.cullStyle, CULL_STYLES, std::size(CULL_STYLES));
        ImGui::Checkbox("Live Playback", &__Live);
    }

    if (__Live)
    {
        __Time = __Latest;
        __Latest = World::GetTime();
    }

    if (ImGui::SliderFloat("Time Code", &__Time, 0.f, __Latest))
    {
        __Live = false;
    }

    __Params.frame = __Time;

    ImGui::End();
}

void dt::Parameter::__draw_render_parameter()
{
    if (ImGui::BeginMenu("Parameter"))
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
            _Latest = World::GetTime();
        }

        if (ImGui::SliderFloat("Time Code", &_Time, 0.f, _Latest))
        {
            _Live = false;
        }

        _Params.frame = _Time;

        ImGui::EndMenu();
    }
}