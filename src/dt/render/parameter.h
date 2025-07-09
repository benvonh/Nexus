#pragma once

#include "pxr/usdImaging/usdImagingGL/renderParams.h"

namespace dt
{
    class Parameter
    {
    public:
        Parameter();

        static void Draw();

    protected:
        void __draw_render_parameter();

        bool _Live = true;
        float _Time = 0.f;
        float _Latest = 0.f;

        pxr::UsdImagingGLRenderParams _Params;

    private:
        static inline bool __Live = true;
        static inline float __Time = 0.f;
        static inline float __Latest = 0.f;
        static inline pxr::UsdImagingGLRenderParams __Params;
    };
}