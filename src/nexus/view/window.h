#pragma once

#include "filedialog.h"

#include "panel/property.h"
#include "panel/scene_hierarchy.h"
#include "panel/viewports.h"

#include "nexus/exception.h"
#include "nexus/logging.h"

#include "SDL3/SDL.h"
#include "SDL3/SDL_opengl.h"

namespace Nexus
{
    class Window : Logger<"Window">
    {
    public:
        Window();
        ~Window() noexcept(false);

        operator bool() const noexcept { return m_Live; }

        void show_exception(const Exception &);

        // void set_vsync(bool);

        void render_frame();

        void handle_events();

    private:
        void _create_layer();
        void _destroy_layer();

        bool m_Live = true;
        bool m_ShowDemo = false;
        bool m_InViewport = false;

        Property m_Property;
        Viewports m_Viewports;
        SceneHierarchy m_SceneHierarchy;
        FileDialog *m_FileDialog = nullptr;

        SDL_Window *m_Window = nullptr;
        SDL_GLContext m_Context = nullptr;
    };
}