#pragma once
#include "dt/scene/manager.hpp"
#include "SDL3/SDL.h"
#include "SDL3/SDL_opengl.h"
#include "pxr/usdImaging/usdImagingGL/engine.h"
#include "pxr/usdImaging/usdImagingGL/renderParams.h"

namespace dt
{
  class Window final
  {
  public:
    Window();
    ~Window();
    void RenderFrame();
    void HandleEvents();
    operator bool() const { return this->operative; }

  private:
    void widgets();
    bool operative = true;
    bool immersive = false;
    bool domeTexture = true;
    SDL_Window *window = nullptr;
    SDL_GLContext context = nullptr;
    pxr::UsdImagingGLEngine *engine = nullptr;
    pxr::UsdImagingGLRenderParams params;
    int cullStyle = 4; // CULL_STYLE_BACK_UNLESS_DOUBLE_SIDED
    int drawMode = 4; // DRAW_SHADED_SMOOTH
  };
}