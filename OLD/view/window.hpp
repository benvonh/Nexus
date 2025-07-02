#pragma once
#include "dt/view/render.hpp"
#include "dt/util.hpp"
#include "SDL3/SDL.h"
#include "SDL3/SDL_opengl.h"

namespace dt
{
  namespace view
  {
    class Window
    {
    public:
      Window();
      ~Window();

      Window(Window &&) = delete;
      Window(const Window &) = delete;
      Window &operator=(Window &&) = delete;
      Window &operator=(const Window &) = delete;

      SDL_Window *operator*() noexcept { return _window; }
      operator bool() const noexcept { return _isLive; }
      void ShowException(const std::exception &e);
      bool Update();

    private:
      bool _isLive = true;
      bool _isInFocus = false;
      bool _showDemo = false;
      SDL_Window *_window;
      SDL_GLContext _context = nullptr;
      Render _render;
    };
  }
}