#pragma once

#include "dt/exception.h"

#include "SDL3/SDL.h"
#include "SDL3/SDL_opengl.h"

#include <functional>

namespace dt
{
    class Window
    {
    public:
        Window();
        ~Window() noexcept(false);

        operator bool() const noexcept { return _Live; }

        SDL_Window *operator*() const noexcept { return _Window; }

        void ShowException(const __exception__ &);

        void SetVSync(bool);

        void StartFrame();

        void FinishFrame();

        void HandleEvents();

    private:
        void __create_layer();
        void __destroy_layer();

        bool _Live = true;
        bool _Show_demo = false;

        SDL_Window *_Window = nullptr;
        SDL_GLContext _Context = nullptr;
    };
}