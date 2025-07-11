#pragma once

#include "dt/event/client.h"
#include "dt/render/render.h"
#include "dt/exception.h"

#include "SDL3/SDL.h"
#include "SDL3/SDL_opengl.h"

namespace dt
{
    class Window : Client
    {
    public:
        Window();
        ~Window() noexcept(false);

        operator bool() const noexcept { return M_Live; }

        SDL_Window *operator*() const noexcept { return M_Window; }

        void show_exception(const Exception &);

        void set_vsync(bool);

        void start_frame();

        void finish_frame();

        void process_events();

    private:
        void create_layer();
        void destroy_layer();

        bool M_Live = true;
        bool M_ShowDemo = false;
        bool M_InViewport = false;

        SDL_Window *M_Window = nullptr;
        SDL_GLContext M_Context = nullptr;
    };
}