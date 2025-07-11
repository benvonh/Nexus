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

        unsigned get_texture();

        void reset();

        void update_size();

        void transform_to_camera();

        void set_camera_path(const pxr::SdfPath &);

        bool FreeCamera = true;

        pxr::GfVec2i Size = {1280, 720};

    private:
        pxr::SdfPath M_CameraPath;

        std::optional<pxr::UsdImagingGLEngine> M_Engine;
    };
}