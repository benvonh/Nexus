#pragma once
#include "dt/gui/sub/controller.h"
#include "dt/gui/sub/parameter.h"
#include "pxr/base/gf/vec2i.h"
#include "pxr/usd/sdf/path.h"
#include "pxr/usdImaging/usdImagingGL/engine.h"
#include <array>
#include <optional>
#include <string_view>

namespace dt
{
    class Render
    {
    public:
        Render(Controller *, Parameter *);

        void draw();

        void reset();

        void enable_free_camera();

    private:
        unsigned __get_texture();

        const std::string_view _C_Name;

        int _Path_index = 0;
        bool _Free_camera = true;
        pxr::GfVec2i _Size = {1280, 720};
        std::optional<pxr::UsdImagingGLEngine> _Engine;
        Controller &_Controller;
        Parameter &_Parameter;

        /**********************************************************
         * All render objects need the path to each camera in the *
         * scene graph which means trversing the stage on every   *
         * update. We cache these paths for effieciency in static *
         * storage.                                               *
         **********************************************************/
    public:
        static void S__cache_paths();

        static constexpr size_t S_CACHE_SIZE = 10;

    private:
        static size_t _S_Cached_paths;
        static std::array<pxr::SdfPath, S_CACHE_SIZE> _S_Paths;
    };
}