#pragma once
#include "digital_twin/plugin.hpp"
#include "dt/base.hpp"
#include "dt/scene/manager.hpp"
#include "dt/view/rendergroup.hpp"
#include "dt/view/filedialog.hpp"
#include "SDL3/SDL.h"
#include "SDL3/SDL_opengl.h"
#include <memory>
#include <optional>
#include <vector>
#include <windows.h>

struct Plugin
{
  HMODULE module;
  dt::IPlugin *instance;
  void (*destroy)(dt::IPlugin *);
};

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

      operator bool() const noexcept { return _isLive; }
      auto GetRaw() const noexcept { return _window; }
      void ShowException(const std::exception &e);
      bool Update();

    private:
      bool demo = false;
      bool _isLive = true;
      bool _isInFocus = false;
      SDL_Window *_window;
      SDL_GLContext context = nullptr;
      RenderGroup _render_group;
      std::vector<Plugin> plugins;
      std::optional<FileDialog> _file_dialog;
    };
  }
}