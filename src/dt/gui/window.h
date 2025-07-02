#pragma once

#include "dt/gui/render.h"
#include "dt/exception.h"

#include "SDL3/SDL_opengl.h"
#include "SDL3/SDL.h"

#include <array>

namespace dt
{
    class Window
    {
    public:
        Window();
        ~Window();

        operator bool() const noexcept { return _Live; }

        void show_exception(const viewable_exception &);
        void render_frame();
        void handle_input();

    private:
        void __draw_menu();

        bool _Live = true;
        bool _Show_demo = false;

        SDL_Window *_Window = nullptr;
        SDL_GLContext _Context = nullptr;

        int _Render_count = 1;
        int _Render_active = -1;
        std::array<Render, 4> _Renders;
    };
}