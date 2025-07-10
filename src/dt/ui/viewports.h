#pragma once

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
    class Viewports
    {
    public:
        Viewports();

        void DrawAll();

        void Reset();

    private:
        void __draw_render(Render &, const char *);

        void __draw_render_menu(Render &);

        void __draw_static_render_controller();

        void __draw_static_render_parameter();

        void __refresh_camera_paths();

        int _Active = 0;

        std::string _Names[DT_NUMBER_OF_RENDERS];

        std::array<Render, DT_NUMBER_OF_RENDERS> _Renders;

        std::vector<pxr::SdfPath> _Camera_paths;
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
