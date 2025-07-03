#pragma once

#include "dt/gui/render.h"
#include "dt/exception.h"

#include "SDL3/SDL_opengl.h"
#include "SDL3/SDL.h"

#include <array>
#include <string>

namespace dt
{
    class Window
    {
        struct FileHandler : FileDialog::Handler
        {
            ~FileHandler() override = default;

            void invoke(const char *, int) override;

            // Copy the path instead of handling it
            // in the callback thread to account for
            // various actions to do with the USD stage.
            static inline std::string Path;
        };

    public:
        Window();
        ~Window();

        operator bool() const noexcept { return _Live; }

        void show_exception(const viewable_exception &);

        void render_frame();

        void handle_input();

    private:
        bool __draw_menu();

        void __modal_update_during_file_dialog();

        bool _Live = true;
        bool _Show_demo = false;

        SDL_Window *_Window = nullptr;
        SDL_GLContext _Context = nullptr;

        int _Render_count = 1;
        int _Render_active = -1;

        std::array<Render, 4> _Renders;
    };
}