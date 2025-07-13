#pragma once

#include "dt/event/client.h"
#include "dt/exception.h"
#include "dt/logging.h"

#include "SDL3/SDL_dialog.h"

#include <functional>
#include <string>

class SDL_Window;

namespace dt
{
    class FileDialog : Client
    {
    public:
        FileDialog(SDL_Window *window)
        {
            log::debug("Initializing file dialog singleton...");
            CHECK(window != nullptr, std::invalid_argument);
            CHECK(S_Window == nullptr, std::logic_error);
            S_Window = window;
        }

        ~FileDialog() noexcept(false)
        {
            log::debug("Invalidating file dialog singleton...");
            CHECK(S_Window != nullptr, std::logic_error);
            S_Window = nullptr;
        }

        static constexpr SDL_DialogFileFilter USD_FILTER[] = {
            {"USD", "usd;usda;usdc;usdz"}};

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

        enum class Option
        {
            FROM_MAIN_THREAD,
            WITHIN_SDL_THREAD
        };

        template <Mode MODE, Option OPT = Option::FROM_MAIN_THREAD, size_t NF>
        static void Show(std::function<void(std::string, int)> &&fn, const SDL_DialogFileFilter (&f)[NF])
        {
            auto *fnPtr = new std::function<void(std::string, int)>(std::move(fn));
            void *ptr = static_cast<void *>(fnPtr);

            if constexpr (MODE == Mode::OPEN)
                SDL_ShowOpenFileDialog(callback<OPT>, ptr, S_Window, f, NF, nullptr, false);

            if constexpr (MODE == Mode::SAVE)
                SDL_ShowSaveFileDialog(callback<OPT>, ptr, S_Window, f, NF, nullptr);
        }

    private:
        template <Option OPT>
        static void callback(void *userdata, const char *const *filelist, int filter) noexcept
        {
            auto *fn = reinterpret_cast<std::function<void(std::string, int)> *>(userdata);

            if (filelist == nullptr)
            {
                dt::log::error("File list was null! SDL reports, \"{}\"", SDL_GetError());
                delete fn;
            }
            else if (filelist[0] == nullptr)
            {
                dt::log::debug("Canceled file dialog");
                delete fn;
            }
            else
            {
                std::string path(filelist[0]);
                dt::log::debug("Selected file '{}'", path);

                if constexpr (OPT == Option::FROM_MAIN_THREAD)
                {
                    Client::Queue(
                        [fn = std::move(fn), path, filter]()
                        {
                            (*fn)(path, filter);
                            delete fn;
                        });
                }
                if constexpr (OPT == Option::WITHIN_SDL_THREAD)
                {
                    (*fn)(path, filter);
                    delete fn;
                }
            }
        }

        static inline SDL_Window *S_Window = nullptr;
    };
}