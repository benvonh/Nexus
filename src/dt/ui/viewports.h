#pragma once

#include "dt/event/client.h"
#include "dt/render/render.h"

#include "pxr/usd/sdf/path.h"

#include <array>
#include <string>
#include <vector>

#ifndef DT_NUMBER_OF_RENDERS
#define DT_NUMBER_OF_RENDERS 4
#endif

namespace dt
{
    class Viewports : Client
    {
    public:
        Viewports();

        void draw();

    private:
        void draw_main_menu();

        void draw_render(size_t index);

        void draw_render_menu(Render &render);

        void draw_static_render_controller();

        void draw_static_render_parameter();

        void refresh_camera_paths();

        // Index of render that consumes input
        int M_Captured = -1;

        // Number of renders that are active
        size_t M_Active = 1;

        // Index of camera path for each render
        int M_CameraIndices[DT_NUMBER_OF_RENDERS];

        // Name of each render
        std::string M_RenderNames[DT_NUMBER_OF_RENDERS];

        // The renders
        std::array<Render, DT_NUMBER_OF_RENDERS> M_Renders;

        // Path of each camera in the scene
        std::vector<pxr::SdfPath> M_CameraPaths;
    };
}