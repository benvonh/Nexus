#pragma once
#include "dt/gui/sub/parameter.h"
#include "dt/gui/sub/controller.h"
#include "dt/gui/sub/render.h"
#include "dt/exception.h"
#include "SDL3/SDL.h"
#include "SDL3/SDL_opengl.h"

namespace dt
{
    class Window
    {
    public:
        Window();
        ~Window();

        Window(Window &&) = delete;
        Window(const Window &) = delete;
        Window &operator=(Window &&) = delete;
        Window &operator=(const Window &) = delete;

        constexpr operator bool() const noexcept { return _IsLive; }
        void show_exception(const viewable_exception &);
        void render_frame();
        void handle_input();

    private:
        void __draw_menu();

        int _NoRenders = 1;
        bool _IsLive = true;
        bool _ShowDemo = false;
        SDL_Window *_Window = nullptr;
        SDL_GLContext _Context = nullptr;
        Controller _Controller;
        Parameter _Parameter;
        Render _Render{&_Controller, &_Parameter};
    };
}