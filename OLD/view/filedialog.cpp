#include "dt/view/filedialog.hpp"
#include "dt/util.hpp"

dt::view::FileDialog::FileDialog(SDL_Window *window)
{
  if (window == nullptr)
    throw std::invalid_argument(exfmt("Window parameter was null!"));

  if (_window != nullptr)
    throw std::logic_error(exfmt("Attempted to re-initialize a singleton"));

  _window = window;
  log::debug("Initialized singleton (window={:p})", _window);
}

dt::view::FileDialog::~FileDialog()
{
  if (_window == nullptr)
    throw std::logic_error(exfmt("Unexpected null value for static member (window)"));

  _window = nullptr;
}

void dt::view::FileDialog::_Callback(void *impl, const char *const *path, int _)
{
  if (path == nullptr)
  {
    log::error("File list was null! SDL reports, \"{}\"", SDL_GetError());
  }
  else if (path[0] == nullptr)
  {
    log::debug("Canceled file dialog");
  }
  else
  {
    if (_ > 1)
    {
      log::alert("Handling multiple files is not implemented!");
    }
    log::debug("Selected file '{}'", path[0]);
    auto &handle_file = *static_cast<HandleFile *>(impl);
    handle_file(path[0]);
  }
}