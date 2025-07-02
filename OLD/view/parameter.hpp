#pragma once
#include "dt/scene/manager.hpp"
#include "pxr/usdImaging/usdImagingGL/renderParams.h"
#include "imgui.h"

namespace dt
{
  namespace view
  {
    class Parameter
    {
      using S = Parameter;

    public:
      static void Draw()
      {
        ImGui::Checkbox("Lighting", &S::params.enableLighting);
        ImGui::Checkbox("Scene Lights", &S::params.enableSceneLights);
        ImGui::Checkbox("Scene Materials", &S::params.enableSceneMaterials);
        ImGui::Checkbox("Show Guides", &S::params.showGuides);
        ImGui::Checkbox("Show Proxy", &S::params.showProxy);
        ImGui::Checkbox("Show Render", &S::params.showRender);
        ImGui::Checkbox("Force Refresh", &S::params.forceRefresh);
        ImGui::Checkbox("Sample Alpha to Coverage", &S::params.enableSampleAlphaToCoverage);
        ImGui::Checkbox("Gamma Correct Colors", &S::params.gammaCorrectColors);
        ImGui::ColorEdit4("Clear Color", &S::params.clearColor[0]);
        ImGui::SliderFloat("Complexity", &S::params.complexity, 1.f, 1.5f, "%.1f");
        ImGui::Combo("Draw Mode", (int *)&S::params.drawMode, DRAW_MODES, std::size(DRAW_MODES));
        ImGui::Combo("Cull Style", (int *)&S::params.cullStyle, CULL_STYLES, std::size(CULL_STYLES));
        ImGui::Checkbox("Live Playback", &S::live);

        if (S::live)
        {
          S::time = latest;
          S::latest = scene::Manager::GetTime();
        }

        if (ImGui::SliderFloat("Time Code", &S::time, 0.f, latest))
        {
          S::live = false;
        }
        S::params.frame = S::time;
      }

    protected:
      static constexpr pxr::UsdImagingGLRenderParams &Params() noexcept
      {
        return S::params;
      }

      static constexpr pxr::UsdImagingGLRenderParams Default() noexcept
      {
        pxr::UsdImagingGLRenderParams params;
        S::params.enableLighting = true;
        S::params.enableSceneLights = true;
        S::params.enableSceneMaterials = true;
        S::params.showGuides = false;
        S::params.showProxy = false;
        S::params.showRender = false;
        S::params.forceRefresh = false;
        S::params.enableSampleAlphaToCoverage = true;
        S::params.gammaCorrectColors = false;
        S::params.clearColor = pxr::GfVec4f(0.5f, 0.5f, 0.5f, 1.f);
        S::params.complexity = 1.f;
        S::params.drawMode = pxr::UsdImagingGLDrawMode::DRAW_SHADED_SMOOTH;
        S::params.cullStyle = pxr::UsdImagingGLCullStyle::CULL_STYLE_BACK_UNLESS_DOUBLE_SIDED;
        S::params.frame = 0;
        return params;
      }

    private:
      static inline bool live = true;
      static inline float time = 0.f;
      static inline float latest = 0.f;
      static inline auto params = S::Default();

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
    };
  }
}