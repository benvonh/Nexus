#pragma once

#include "dt/event/client.h"

#include "SDL3/SDL_dialog.h"

#include <functional>
#include <string>

class SDL_Window;

namespace dt
{
    class Application;
    class FileDialog : Client
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

        ///
        /// To help index the filter extension
        ///
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

        template <Mode MODE, size_t NF>
        static void Show(std::function<void(std::string, int)> &&fn, const SDL_DialogFileFilter (&f)[NF])
        {
            auto *fnPtr = new std::function<void(std::string, int)>(std::move(fn));
            void *userdata = static_cast<void *>(fnPtr);

            if constexpr (MODE == Mode::OPEN)
                SDL_ShowOpenFileDialog(callback, userdata, S__Window, f, NF, nullptr, false);

            if constexpr (MODE == Mode::SAVE)
                SDL_ShowSaveFileDialog(callback, userdata, S__Window, f, NF, nullptr);
        }

    private:
        FileDialog(SDL_Window *window);

        ~FileDialog() noexcept(false);

        static void callback(void *, const char *const *, int) noexcept;

        static inline SDL_Window *S__Window = nullptr;
    };
}