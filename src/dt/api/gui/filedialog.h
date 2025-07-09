#pragma once

#include "dt/api/api.h"
#include "dt/logging.h"
#include "dt/exception.h"

#include "SDL3/SDL_video.h"
#include "SDL3/SDL_dialog.h"

namespace dt
{
    class Application;

    template <typename E>
    class DT_API FileDialog final
    {
        friend class Application;

    public:
        static constexpr SDL_DialogFileFilter USD_FILTER[] = {
            {"USD Files", "usd;usda;usdc;usdz"}};

        static constexpr SDL_DialogFileFilter IMAGE_FILTER[] = {
            {"PNG", "png"},
            {"BMP", "bmp"},
            {"TGA", "tga"},
            {"JPG", "jpg"},
            {"HDR", "hdr"}};

        enum ImageFormat
        {
            PNG,
            BMP,
            TGA,
            JPG,
            HDR
        };

        enum class Mode
        {
            OPEN,
            SAVE
        };

        enum class Filter
        {
            IMAGE,
            USD
        };

        template <Mode MODE>
        static void Show(Filter filter)
        {
            SDL_DialogFileFilter *f = nullptr;

            switch (filter)
            {
            case Filter::USD:
                f = USD_FILTER;
                break;
            case Filter::IMAGE:
                f = IMAGE_FILTER;
                break;
            }

            int nf = sizeof(f) / sizeof(SDL_DialogFileFilter);

            if constexpr (MODE == Mode::OPEN)
                SDL_ShowOpenFileDialog(__callback, nullptr, _Window, f, nf, nullptr, false);

            if constexpr (MODE == Mode::SAVE)
                SDL_ShowSaveFileDialog(__callback, nullptr, _Window, f, nf, nullptr);
        }

    private:
        FileDialog(SDL_Window *window) : _Window(window)
        {
            DT_THROW_IF(window == nullptr, std::invalid_argument);
        }

        ~FileDialog() noexcept { _Window = nullptr; }

        static void __callback(void *, const char *const *, int) noexcept
        {
            if (path == nullptr)
                log::error("File list is null! SDL reports, \"{}\"", SDL_GetError());

            else if (path[0] == nullptr)
                log::debug("Canceled file dialog");

            else
            {
                // TODO
                // [[likely]]
                // __Handler->invoke(path[0], ext);
            }
        }

        static inline SDL_Window *_Window = nullptr;
    };
}