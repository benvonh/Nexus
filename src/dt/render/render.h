#pragma once

#include "dt/render/controller.h"
#include "dt/render/parameter.h"

#include "pxr/base/gf/vec2i.h"
#include "pxr/usd/sdf/path.h"
#include "pxr/usdImaging/usdImagingGL/engine.h"

#include <string>
#include <optional>

namespace dt
{
    class Render : public Controller, public Parameter
    {
    public:
        [[nodiscard]]
        unsigned operator()();

        [[nodiscard]]
        unsigned operator()(const pxr::SdfPath &path);

        void Reset();

        void UpdateSize();

        void EnableFreeCamera(const pxr::SdfPath &path);

        void DisableFreeCamera();

        bool free_camera = true;

        pxr::GfVec2i size = {1280, 720};

    private:
        unsigned __get_texture();

        std::optional<pxr::UsdImagingGLEngine> _Engine;
    };
}