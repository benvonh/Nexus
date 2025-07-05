#pragma once

#include "dt/exception.h"
#include "dt/logging.h"
#include "dt/utility.h"

#include "SDL3/SDL_dialog.h"
#include "SDL3/SDL_video.h"

#include <array>
#include <atomic>

namespace dt
{
    class FileDialog
    {
    public:
        struct Handler
        {
            virtual ~Handler() = default;

            virtual void invoke(const char *path, int ext) = 0;
        };

        enum class Mode
        {
            OPEN,
            SAVE
        };

        template <Mode MODE, size_t NF>
        static void Show(Ptr<Handler> handler, std::array<SDL_DialogFileFilter, NF> filter)
        {
            if (__Window == nullptr)
                throw exception("Window is not set!");

            __Handler = std::move(handler);

            Done = false;

            if constexpr (MODE == Mode::OPEN)
                SDL_ShowOpenFileDialog(__Callback,
                                       nullptr,
                                       __Window,
                                       filter.data(),
                                       NF,
                                       nullptr,
                                       false);

            if constexpr (MODE == Mode::SAVE)
                SDL_ShowSaveFileDialog(__Callback,
                                       nullptr,
                                       __Window,
                                       filter.data(),
                                       NF,
                                       nullptr);
        }

        static void SetWindow(SDL_Window *window) noexcept { __Window = window; }

        enum ImageFormat
        {
            PNG,
            BMP,
            TGA,
            JPG,
            HDR
        };

        static inline constexpr std::array<SDL_DialogFileFilter, 5> IMAGE_FILTER = {
            SDL_DialogFileFilter{"PNG", "png"},
            SDL_DialogFileFilter{"BMP", "bmp"},
            SDL_DialogFileFilter{"TGA", "tga"},
            SDL_DialogFileFilter{"JPG", "jpg"},
            SDL_DialogFileFilter{"HDR", "hdr"}};

        static inline constexpr std::array<SDL_DialogFileFilter, 1> USD_FILTER = {
            SDL_DialogFileFilter{"USD Files", "usd;usda;usdc;usdz"}};

        static inline std::atomic<bool> Done = false;

    private:
        static void __Callback(void *_, const char *const *path, int ext) noexcept
        {
            if (path == nullptr)
                log::error("File list was null! SDL reports, \"{}\"", SDL_GetError());

            else if (path[0] == nullptr)
                log::debug("Canceled file dialog");

            else
            {
                if (!__Handler)
                    log::error("File dialog handler is null!");

                else [[likely]]
                    __Handler->invoke(path[0], ext);
            }
            Done = true;
        }

        // Should be initialized to the parent window to become modal
        static inline SDL_Window *__Window = nullptr;
        // There should only be one valid handler when a file dialog is shown anyway
        static inline Ptr<Handler> __Handler;
    };
}