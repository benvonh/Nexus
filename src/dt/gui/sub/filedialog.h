#pragma once

#include "SDL3/SDL_dialog.h"
#include "SDL3/SDL_video.h"

namespace dt
{
    class FileDialog
    {
    public:
        FileDialog(SDL_Window *window) : _window(window)
        {
            if (_window == nullptr)
                throw exception("SDL_Window is null");
        }

        ~FileDialog() = default;

        FileDialog(FileDialog &&) = delete;
        FileDialog(const FileDialog &) = delete;
        FileDialog &operator=(FileDialog &&) = delete;
        FileDialog &operator=(const FileDialog &) = delete;

        template <int NF>
        void Show(SDL_DialogFileFilter filter[NF])
        {
            SDL_ShowOpenFileDialog(_Callback, _window, filter, NF, nullptr, false);
        }

    private:
        static inline SDL_Window *__Window = nullptr;
    }
}