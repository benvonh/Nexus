#pragma once
#include "dt/view/camera.hpp"
#include "dt/scene/manager.hpp"
#include "pxr/imaging/hgiGL/texture.h"
#include "pxr/usdImaging/usdImagingGL/engine.h"
#include "pxr/usdImaging/usdImagingGL/renderParams.h"
#include "SDL3/SDL_opengl.h"
#include "SDL3/SDL.h"

namespace dt
{
  namespace view
  {
    class Window
    {
      struct Render
      {
        unsigned texture = 0;
        unsigned x = 0, y = 0;
        unsigned w = 1280, h = 720;
      };

    public:
      Window();
      ~Window();
      void RenderFrame();
      void HandleEvents();
      inline operator bool() const { return this->operative; }

    private:
      void imgui();
      void new_usd();
      void open_usd();
      void export_usd();

      int drawMode = 4; // DRAW_SHADED_SMOOTH
      int cullStyle = 4; // CULL_STYLE_BACK_UNLESS_DOUBLE_SIDED
      bool operative = true;
      bool immersive = false;
      bool domeTexture = true;
      SDL_Window *window = nullptr;
      SDL_GLContext context = nullptr;
      pxr::UsdImagingGLEngine *engine = nullptr;
      pxr::UsdImagingGLRenderParams params;
      Render render;
      Camera camera;
    };
  }
}