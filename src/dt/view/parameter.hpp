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
      using self = Parameter;

    public:
      static void Draw()
      {
        ImGui::Checkbox("Lighting", &self::params.enableLighting);
        ImGui::Checkbox("Scene Lights", &self::params.enableSceneLights);
        ImGui::Checkbox("Scene Materials", &self::params.enableSceneMaterials);
        ImGui::Checkbox("Show Guides", &self::params.showGuides);
        ImGui::Checkbox("Show Proxy", &self::params.showProxy);
        ImGui::Checkbox("Show Render", &self::params.showRender);
        ImGui::Checkbox("Force Refresh", &self::params.forceRefresh);
        ImGui::Checkbox("Sample Alpha to Coverage", &self::params.enableSampleAlphaToCoverage);
        ImGui::Checkbox("Gamma Correct Colors", &self::params.gammaCorrectColors);
        ImGui::ColorEdit4("Clear Color", &self::params.clearColor[0]);
        ImGui::SliderFloat("Complexity", &self::params.complexity, 1.f, 1.5f, "%.1f");
        ImGui::Combo("Draw Mode", (int *)&self::params.drawMode, DRAW_MODES, DRAW_MODES_SIZE);
        ImGui::Combo("Cull Style", (int *)&self::params.cullStyle, CULL_STYLES, CULL_STYLES_SIZE);
        ImGui::Checkbox("Live Playback", &self::live);

        if (self::live)
        {
          self::time = latest;
          self::latest = scene::Manager::GetTime();
        }

        if (ImGui::SliderFloat("Time Code", &self::time, 0.f, latest))
        {
          self::live = false;
        }

        self::params.frame = self::time;
      }

    protected:
      static const pxr::UsdImagingGLRenderParams &Params() noexcept
      {
        return self::params;
      }

      static pxr::UsdImagingGLRenderParams Default() noexcept
      {
        pxr::UsdImagingGLRenderParams params;
        self::params.enableLighting = true;
        self::params.enableSceneLights = true;
        self::params.enableSceneMaterials = true;
        self::params.showGuides = false;
        self::params.showProxy = false;
        self::params.showRender = false;
        self::params.forceRefresh = false;
        self::params.enableSampleAlphaToCoverage = true;
        self::params.gammaCorrectColors = false;
        self::params.clearColor = pxr::GfVec4f(0.52734375f, 0.8046875f, 0.91796875f, 1.f);
        self::params.complexity = 1.f;
        self::params.drawMode = pxr::UsdImagingGLDrawMode::DRAW_SHADED_SMOOTH;
        self::params.cullStyle = pxr::UsdImagingGLCullStyle::CULL_STYLE_BACK_UNLESS_DOUBLE_SIDED;
        self::params.frame = 0;
        return params;
      }

    private:
      static inline bool live = true;
      static inline float time = 0.f;
      static inline float latest = 0.f;
      static inline auto params = self::Default();

      static inline const char *DRAW_MODES[] = {
          "Points",
          "Wireframe",
          "Wireframe on Surface",
          "Shaded Flat",
          "Shaded Smooth",
          "Geometry Only",
          "Geometry Flat",
          "Geometry Smooth"};

      static inline const char *CULL_STYLES[] = {
          "No Opinion",
          "Nothing",
          "Back",
          "Front",
          "Back Unless Double-Sided"};

      static constexpr size_t DRAW_MODES_SIZE = std::size(DRAW_MODES);
      static constexpr size_t CULL_STYLES_SIZE = std::size(CULL_STYLES);
    };
  }
}