#pragma once
#include "pxr/usdImaging/usdImagingGL/renderParams.h"

namespace dt
{
    class Parameter
    {
    public:
        Parameter();

        void draw();

        const auto &get_params() const { return _Params; }

    private:
        bool _Live = true;
        float _Time = 0.f;
        float _Latest = 0.f;
        pxr::UsdImagingGLRenderParams _Params;
    };
}