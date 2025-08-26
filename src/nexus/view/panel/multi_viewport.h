#pragma once

#include "nexus/render/render.h"

#include "pxr/usd/sdf/path.h"

#include <array>
#include <string>
#include <vector>

#ifndef VIEWPORT_RENDER_COUNT
#define VIEWPORT_RENDER_COUNT 4
#endif

namespace Nexus
{
    class MultiViewport final
    {
    public:
        MultiViewport();

        void start_engine();
        void stop_engine();
        void no_capture() { m_Captured = -1; }
        void draw();

        [[nodiscard]]
        auto &get_active_render()
        {
            return m_Renders[m_Captured];
        }

    private:
        void _draw_main_menu();
        void _draw_render(std::size_t index);
        void _draw_render_menu(Render &render);
        void _draw_static_render_controller();
        void _draw_static_render_parameter();
        void _refresh_camera_paths();

    private:
        // Index of render that wants input
        int m_Captured = -1;

        // Number of renders that are active
        std::size_t m_Active = 1;

        // Index of camera path for each render
        int m_CameraIndices[VIEWPORT_RENDER_COUNT];

        // Name of each render
        std::string m_RenderNames[VIEWPORT_RENDER_COUNT];

        // The render instances
        std::array<Render, VIEWPORT_RENDER_COUNT> m_Renders;

        // Path of each camera in the scene
        std::vector<pxr::SdfPath> m_CameraPaths;
    };
}