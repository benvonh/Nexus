#pragma once
#include "dt/base.hpp"
#include "SDL3/SDL_dialog.h"
#include "SDL3/SDL_video.h"
#include <string>

namespace dt
{
  namespace view
  {
    class FileDialog
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
        virtual void operator()(const std::string &path)
      };

      enum class Mode
      {
        OPEN,
        SAVE
      };

      static inline constexpr SDL_DialogFileFilter usdFilter[] = {
          {"USD Files", "usd;usda;usdc;usdz"},
          {"All Files", "*"}};

      template <Mode M>
      static void Show(HandleFile *handle, SDL_DialogFileFilter filter[])
      {
        if (_window == nullptr)
          throw exception("Window is null! Did you forget to initialize?");

        constexpr auto nf = std::size(FileDialog::usdFilter);

        if constexpr (M == Mode::OPEN)
        {
          SDL_ShowOpenFileDialog(
              _Callback,
              handle,
              window,
              FileDialog::usdFilter,
              nf, nullptr, false);
        }
        if constexpr (M == Mode::SAVE)
        {
          SDL_ShowSaveFileDialog(
              _Callback,
              handle,
              window,
              FileDialog::usdFilter,
              nf, nullptr);
        }
      }

    private:
      static void _Callback(void *impl, const char *const *file, int _);

      static inline SDL_Window *_window = nullptr;
    };
  }
}