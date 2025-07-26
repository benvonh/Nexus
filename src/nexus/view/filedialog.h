#pragma once

#include "nexus/event/client.h"
#include "nexus/exception.h"
#include "nexus/logging.h"

#include "SDL3/SDL_dialog.h"

#include <functional>
#include <string>

class SDL_Window;

namespace Nexus
{
    class Window;

    class FileDialog : Logger<"File Dialogue">
    {
        friend class Nexus::Window;

        using Callback = std::function<void(std::string, int)>;

    public:
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

        enum class Thread
        {
            MAIN,
            SDL
        };

        template <Mode MODE, Thread Th = Thread::MAIN, size_t NF>
        static void Show(Callback &&fn, const SDL_DialogFileFilter (&f)[NF])
        {
            s_Callback = std::move(fn);

            if constexpr (MODE == Mode::OPEN)
                SDL_ShowOpenFileDialog(callback<Th>, nullptr, s_Window, f, NF, nullptr, false);

            if constexpr (MODE == Mode::SAVE)
                SDL_ShowSaveFileDialog(callback<Th>, nullptr, s_Window, f, NF, nullptr);
        }

    private:
        FileDialog(SDL_Window *window)
        {
            LOG_BASIC("Initializing FileDialog...");

            if (window == nullptr)
                throw std::invalid_argument("Window is null");

            if (s_Window != nullptr)
                throw std::logic_error("FileDialog already initialized");

            s_Window = window;
        }

        ~FileDialog() noexcept(false)
        {
            if (s_Window == nullptr)
                throw std::logic_error("FileDialog not initialized");

            s_Window = nullptr;
        }

        template <Thread Th>
        static void callback(void *userdata, const char *const *filelist, int filter) noexcept
        {
            if (filelist == nullptr)
            {
                LOG_ERROR("File list is null! SDL reports, \"{}\"", SDL_GetError());
            }
            else if (filelist[0] == nullptr)
            {
                LOG_BASIC("Canceled file dialog");
            }
            else
            {
                std::string path(filelist[0]);
                LOG_EVENT("Selected file '{}'", path);

                if constexpr (Th == Thread::MAIN)
                {
                    Client::Queue(
                        [fn = std::move(s_Callback), path, filter]()
                        {
                            fn(path, filter);
                        });
                }
                if constexpr (Th == Thread::SDL)
                {
                    s_Callback(path, filter);
                }
            }
        }

        static inline Callback s_Callback;

        static inline SDL_Window *s_Window = nullptr;
    };
}