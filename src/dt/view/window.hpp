#pragma once
#include "digital_twin/base.hpp"
#include "digital_twin/plugin.hpp"
#include "dt/scene/manager.hpp"
#include "dt/view/render.hpp"
#include "SDL3/SDL.h"
#include "SDL3/SDL_opengl.h"
#include <atomic>
#include <string>
#include <thread>
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
      static constexpr auto &OpenFile = SDL_ShowOpenFileDialog;
      static constexpr auto &SaveFile = SDL_ShowSaveFileDialog;

    public:
      Window();
      ~Window();
      Window(Window &&) = delete;
      Window(const Window &) = delete;
      Window &operator=(Window &&) = delete;
      Window &operator=(const Window &) = delete;
      void operator()();
      bool Update();

    private:
      bool demo = false;
      int active = -1;
      SDL_Window *window = nullptr;
      SDL_GLContext context = nullptr;
      std::vector<Render> renders;
      std::vector<Plugin> plugins;

      void DefaultRender()
      {
        this->renders.clear();
        this->renders.emplace_back("Viewport");
      }

      struct FileDialogCallbackData
      {
        bool cancel = false;
        std::string file;
        std::atomic<bool> done = false;
      };

      static void FileDialogCallback(void *userdata, const char *const *filelist, int filter)
      {
        auto *data = static_cast<FileDialogCallbackData *>(userdata);

        if (filelist == nullptr)
        {
          log::error("File list is null: {}", SDL_GetError());
        }
        else if (filelist[0] == nullptr)
        {
          log::alert("Canceled file dialog");
          data->cancel = true;
        }
        else
        {
          log::event("File selected: {}", filelist[0]);
          data->file = filelist[0];
        }
        data->done = true;
      }

      template <scene::Action A>
      void HandleFile()
      {
        FileDialogCallbackData data;

        const SDL_DialogFileFilter filters[] = {
            {"USD Files", "usd;usda;usdc;usdz"},
            {"All Files", "*"}};

        constexpr auto size = std::size(filters);

        if constexpr (A == scene::Action::NEW)
        {
          SaveFile(FileDialogCallback, (void *)&data, this->window, filters, size, nullptr);
        }
        if constexpr (A == scene::Action::OPEN)
        {
          OpenFile(FileDialogCallback, (void *)&data, this->window, filters, size, nullptr, false);
        }
        if constexpr (A == scene::Action::SAVE)
        {
          scene::Manager::HandleStage<scene::Action::SAVE>();
          return;
        }
        if constexpr (A == scene::Action::EXPORT)
        {
          SaveFile(FileDialogCallback, (void *)&data, this->window, filters, size, nullptr);
        }

        while (this->Update() && !data.done)
          ;

        if (data.cancel)
          return;

        if constexpr (A == scene::Action::NEW)
        {
          scene::Manager::HandleStage<scene::Action::NEW>(data.file);
          this->DefaultRender();
        }
        if constexpr (A == scene::Action::OPEN)
        {
          scene::Manager::HandleStage<scene::Action::OPEN>(data.file);
          this->DefaultRender();
        }
        if constexpr (A == scene::Action::EXPORT)
        {
          scene::Manager::HandleStage<scene::Action::EXPORT>(data.file);
        }
      }
    };
  }
}