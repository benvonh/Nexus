#pragma once
#include "dt/declspec.hpp"
#include "SDL3/SDL_dialog.h"
#include "SDL3/SDL_video.h"

namespace dt
{
  namespace view
  {
    class DT_API FileDialog
    {
    public:
      FileDialog(SDL_Window *window);
      ~FileDialog();

      FileDialog(FileDialog &&) = delete;
      FileDialog(const FileDialog &) = delete;
      FileDialog &operator=(FileDialog &&) = delete;
      FileDialog &operator=(const FileDialog &) = delete;

      struct HandleFile
      {
        virtual void operator()(const std::string &path) = 0;
      };

      enum class Mode
      {
        OPEN,
        SAVE
      };

      template <Mode M, int NF>
      void Show(HandleFile *handle, SDL_DialogFileFilter filter[NF])
      {
        if (_window == nullptr)
          throw exception("Singleton was not initialized!");
        
        if constexpr (M == Mode::OPEN)
        {
          SDL_ShowOpenFileDialog(
              _Callback,
              handle,
              _window,
              filter,
              NF,
              nullptr,
              false);
        }
        if constexpr (M == Mode::SAVE)
        {
          SDL_ShowSaveFileDialog(
              _Callback,
              handle,
              _window,
              filter,
              NF,
              nullptr);
        }
      }

      static inline constexpr SDL_DialogFileFilter usd_filter[] = {
          {"USD Files", "usd;usda;usdc;usdz"}};
      // {"USD Files", "usd;usda;usdc;usdz"},
      // {"All Files", "*"}};

    private:
      static void _Callback(void *, const char *const *, int);

      // TODO: Does this have to be not inline?
      static inline SDL_Window *_window;
    };
  }
}