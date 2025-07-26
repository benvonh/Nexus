#pragma once

#include "pxr/usdImaging/usdImagingGL/renderParams.h"

namespace Nexus
{
    struct Parameter
    {
        Parameter()
        {
            Params.clearColor = pxr::GfVec4f(0.5f, 0.5f, 0.5f, 1.f);
            Params.complexity = 1.f;
            Params.cullStyle = pxr::UsdImagingGLCullStyle::CULL_STYLE_BACK_UNLESS_DOUBLE_SIDED;
            Params.drawMode = pxr::UsdImagingGLDrawMode::DRAW_SHADED_FLAT;
            Params.enableLighting = true;
            Params.enableSampleAlphaToCoverage = true;
            Params.enableSceneLights = true;
            Params.enableSceneMaterials = true;
            Params.forceRefresh = false;
            Params.frame = 0;
            Params.gammaCorrectColors = false;
            Params.showGuides = false;
            Params.showProxy = false;
            Params.showRender = false;
            PARAMS = Params;
        }

        bool Live = true;
        float Time = 0.f;

        pxr::UsdImagingGLRenderParams Params;

        static inline bool LIVE = true;
        static inline float TIME = 0.f;
        static inline float LATEST = 0.f;

        static inline pxr::UsdImagingGLRenderParams PARAMS;
    };
}