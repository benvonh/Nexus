#pragma once

#include "pxr/usdImaging/usdImagingGL/renderParams.h"

namespace dt
{
    struct Parameter
    {
        Parameter()
        {
            this->params.clearColor = pxr::GfVec4f(0.5f, 0.5f, 0.5f, 1.f);
            this->params.complexity = 1.f;
            this->params.cullStyle = pxr::UsdImagingGLCullStyle::CULL_STYLE_BACK_UNLESS_DOUBLE_SIDED;
            this->params.drawMode = pxr::UsdImagingGLDrawMode::DRAW_SHADED_FLAT;
            this->params.enableLighting = true;
            this->params.enableSampleAlphaToCoverage = true;
            this->params.enableSceneLights = true;
            this->params.enableSceneMaterials = true;
            this->params.forceRefresh = false;
            this->params.frame = 0;
            this->params.gammaCorrectColors = false;
            this->params.showGuides = false;
            this->params.showProxy = false;
            this->params.showRender = false;
            this->Params = this->params;
        }

        bool live = true;
        float time = 0.f;
        float latest = 0.f;

        pxr::UsdImagingGLRenderParams params;

        static inline bool Live = true;
        static inline float Time = 0.f;
        static inline float Latest = 0.f;

        static inline pxr::UsdImagingGLRenderParams Params;
    };
}