#pragma once
#include "pxr/usdImaging/usdImagingGL/renderParams.h"

namespace dt
{
    class Parameter
    {
    public:
        Parameter();

        const auto &get_params() const { return _Params; }

        void draw();

        static void Draw();

    private:
        bool _Live = true;
        float _Time = 0.f;
        float _Latest = 0.f;

        pxr::UsdImagingGLRenderParams _Params;

        static inline bool __Live = true;
        static inline float __Time = 0.f;
        static inline float __Latest = 0.f;
        static inline pxr::UsdImagingGLRenderParams __Params;
    };
}