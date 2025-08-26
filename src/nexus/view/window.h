#pragma once

#include "filedialog.h"

#include "nexus/exception.h"
#include "nexus/view/panel/multi_viewport.h"
#include "nexus/view/panel/prim_property.h"
#include "nexus/view/panel/scene_hierarchy.h"

#include "SDL3/SDL.h"
#include "SDL3/SDL_opengl.h"

#include <memory>

namespace Nexus
{
    class Window final
    {
        class ViewportCaptureMode
        {
        public:
            template <bool CAPTURE>
            void set(SDL_Window *window);

            operator bool() const noexcept { return m_Captured; }

        private:
            bool m_Captured = false;
        };

    public:
        Window();
        ~Window() noexcept(false);

        operator bool() const noexcept { return m_Live; }

        void show_exception(const Exception &);

        void render_frame();
        void handle_events();

    private:
        void _create_layer();
        void _destroy_layer();
        void _draw_low_level();

    private:
        /* Internal Flags */
        bool m_Live = true;
        bool m_ShowDemo = false;

        /* View Panels */
        PrimProperty m_PrimProperty;
        MultiViewport m_MultiViewport;
        SceneHierarchy m_SceneHierarchy;

        /* File Dialog Singleton */
        std::unique_ptr<FileDialog> m_FileDialog;

        /* SDL Data Structures */
        SDL_Window *m_Window = nullptr;
        SDL_GLContext m_Context = nullptr;

        /* Convenient Wrapper */
        ViewportCaptureMode m_ViewportCapture;
    };
}