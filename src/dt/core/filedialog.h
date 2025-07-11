#pragma once

#include "dt/event/client.h"

#include "SDL3/SDL_dialog.h"

#include <functional>
#include <string>

class SDL_Window;

namespace dt
{
    class FileDialog : Client
    {
    public:
        FileDialog(SDL_Window *window);

        ~FileDialog() noexcept(false);

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
            void *ptr = static_cast<void *>(fnPtr);

            if constexpr (MODE == Mode::OPEN)
                SDL_ShowOpenFileDialog(callback, ptr, S_Window, f, NF, nullptr, false);

            if constexpr (MODE == Mode::SAVE)
                SDL_ShowSaveFileDialog(callback, ptr, S_Window, f, NF, nullptr);
        }

    private:
        static void callback(void *, const char *const *, int) noexcept;

        static inline SDL_Window *S_Window = nullptr;
    };
}