#pragma once

#include "dt/gui/sub/controller.h"
#include "dt/gui/sub/parameter.h"
#include "dt/gui/filedialog.h"

#include "pxr/base/gf/vec2i.h"
#include "pxr/usd/sdf/path.h"
#include "pxr/usdImaging/usdImagingGL/engine.h"

#include <optional>
#include <string>
#include <vector>

namespace dt
{
    class Render : public Controller
    {
        struct FileHandler : FileDialog::Handler
        {
            FileHandler(int, int);

            ~FileHandler() override = default;

            void invoke(const char *, int) override;

            static constexpr int STRIDE = 3;

            int Width = 0;
            int Height = 0;

            std::vector<u_char> Pixels;
        };

    public:
        Render();

        bool draw();

        void reset();

        void enable_free_camera();

    private:
        unsigned __get_texture();

        const std::string _NAME;

        int _Path_index = 0;

        bool _Free_camera = true;

        pxr::GfVec2i _Size = {1280, 720};

        std::optional<pxr::UsdImagingGLEngine> _Engine;

        Parameter _Parameter;

        /**********************************************************
         * All render objects need the path to each camera in the *
         * scene graph which means traversing the stage on every  *
         * update. We cache these paths for effieciency in static *
         * storage.                                               *
         **********************************************************/
    public:
        static void CachePaths();

    private:
        static constexpr size_t __CACHE_SIZE = 2;

        static inline size_t __Cached_paths = 0;

        static inline pxr::SdfPath __Paths[__CACHE_SIZE];
    };
}