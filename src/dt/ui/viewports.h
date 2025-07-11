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

        void reset();

    private:
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

// dt::Render::FileHandler::FileHandler(int w, int h)
//     : Width(w), Height(h), Pixels(w * h * 3) {}

// /*============================================================================*/
// void dt::Render::FileHandler::invoke(const char *path, int extension)
// {
//     auto f = std::format("{}.{}", path, FileDialog::IMAGE_FILTER[extension].pattern);

//     log::event("Saving render ({}x{}) as image at '{}'", Width, Height, f);

//     stbi_flip_vertically_on_write(1);

//     switch (extension)
//     {
//     case FileDialog::ImageFormat::PNG:
//         if (!stbi_write_png(f.c_str(), Width, Height, STRIDE, Pixels.data(), Width * STRIDE))
//             log::error("Error saving as PNG");
//         break;
//     case FileDialog::ImageFormat::BMP:
//         if (!stbi_write_bmp(f.c_str(), Width, Height, STRIDE, Pixels.data()))
//             log::error("Error saving as BMP");
//         break;
//     case FileDialog::ImageFormat::TGA:
//         if (!stbi_write_tga(f.c_str(), Width, Height, STRIDE, Pixels.data()))
//             log::error("Error saving as TGA");
//         break;
//     case FileDialog::ImageFormat::JPG:
//         if (!stbi_write_jpg(f.c_str(), Width, Height, STRIDE, Pixels.data(), 100))
//             log::error("Error saving as JPG");
//         break;
//     case FileDialog::ImageFormat::HDR:
//         log::alert("HDR image format is not supported yet, saving as HDR is disabled.");
//         // TODO: Casting char to float is not a good idea
//         // if (!stbi_write_hdr(f.c_str(), Width, Height, STRIDE, reinterpret_cast<float *>(Pixels.data())));
//         //     log::error("Error saving as HDR");
//         break;
//     default:
//         log::error("Unknown image format, got {}", extension);
//     }
// }
