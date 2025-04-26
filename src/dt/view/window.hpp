#pragma once
#include "dt/scene/manager.hpp"
#include "dt/view/camera.hpp"
#include "dt/view/render.hpp"
#include "SDL3/SDL.h"
#include "SDL3/SDL_opengl.h"
#include <atomic>
#include <string>

namespace dt
{
namespace view
{
class Window
{
  static constexpr auto &OpenFile = SDL_ShowOpenFileDialog;
  static constexpr auto &SaveFile = SDL_ShowSaveFileDialog;
  static constexpr auto &MessageBox = SDL_ShowSimpleMessageBox;

public:
  Window();
  ~Window();
  Window(Window &&) = delete;
  Window(const Window &) = delete;
  Window &operator=(Window &&) = delete;
  Window &operator=(const Window &) = delete;
  void operator()(Render &render);

private:
  bool operative = true;
  bool immersive = false;
  Render *render_ptr = nullptr;
  SDL_Window *window = nullptr;
  SDL_GLContext context = nullptr;
  Camera camera;

  void Update();

  template <scene::Action A>
  void HandleFile()
  {
    SDL_Log("Handling file with action %d", static_cast<int>(A));

    struct CallbackData
    {
      std::string file;
      std::atomic<bool> done;
    };

    auto callback = [](void *userdata, const char * const *filelist, int filter)
      {
        if (filelist == nullptr)
        {
          SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, SDL_GetError());
        }
        if (filelist[0] == nullptr)
        {
          SDL_Log("No file");
        }
        else
        {
          SDL_Log("File: %s", filelist[0]);
          CallbackData *data = static_cast<CallbackData *>(userdata);
          data->file = filelist[0];
          data->done = true;
        }
      };

    CallbackData data{.done = false};

    const SDL_DialogFileFilter filters[] = {
      { "USD Files", "usd;usda;usdc;usdz" },
      { "All Files", "*" } };

    constexpr auto size = std::size(filters);

    if constexpr (A == scene::Action::NEW)
    {
      SaveFile(callback, (void *)&data, this->window, filters, size, nullptr);
    }
    if constexpr (A == scene::Action::OPEN)
    {
      OpenFile(callback, (void *)&data, this->window, filters, size, nullptr, false);
    }
    if constexpr (A == scene::Action::SAVE)
    {
      scene::Manager::HandleStage<scene::Action::SAVE>();
      return;
    }
    if constexpr (A == scene::Action::EXPORT)
    {
      SaveFile(callback, (void *)&data, this->window, filters, size, nullptr);
    }

    while (this->operative && !data.done) { this->Update(); }

    if constexpr (A == scene::Action::NEW)
    {
      scene::Manager::HandleStage<scene::Action::NEW>(data.file);
      render_ptr->Reset();
    }
    if constexpr (A == scene::Action::OPEN)
    {
      scene::Manager::HandleStage<scene::Action::OPEN>(data.file);
      render_ptr->Reset();
    }
    if constexpr (A == scene::Action::EXPORT)
    {
      scene::Manager::HandleStage<scene::Action::EXPORT>(data.file);
    }
  }
};
} // namespace view
} // namespace dt