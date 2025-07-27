#pragma once

#include "nexus/render/controller.h"
#include "nexus/render/parameter.h"
#include "nexus/logging.h"

#include "pxr/base/gf/vec2i.h"
#include "pxr/usd/sdf/path.h"
#include "pxr/usdImaging/usdImagingGL/engine.h"

namespace Nexus
{
    class Render : public Controller, public Parameter, Logger<"Render">
    {
        using Engine = pxr::UsdImagingGLEngine;

    public:
        [[nodiscard]]
        unsigned operator()();

        [[nodiscard]]
        unsigned get_texture();

        void reset();

        void update_size();

        void delete_engine();

        void transform_to_camera();

    public:
        pxr::SdfPath CameraPath;

        pxr::GfVec2i Size = {1280, 720};

        bool FreeCamera = true;

    private:
        alignas(Engine) std::byte m_EngineMemorySpace[sizeof(Engine)];

        Engine *m_Engine = nullptr;
    };
}