#include "dt/view/filedialog.hpp"

dt::view::FileDialog::FileDialog(SDL_Window *window)
{
  if (window == nullptr)
    throw std::invalid_argument(exfmt("Window parameter was null!"));

  if (_window != nullptr)
    throw std::logic_error(exfmt("Window member was not null (window={:p}), "
                                 "initialize singleton once!",
                                 _window));

  _window = window;
  log::debug("Initialized FileDialog singleton (window={:p})", _window);
}

dt::view::FileDialog::~FileDialog()
{
  if (_window == nullptr)
    throw std::logic_error(exfmt("Window member was null!"));

  _window = nullptr;
}

void dt::view::FileDialog::_Callback(void *impl, const char *const *file, int _)
{
  if (file == nullptr)
  {
    log::error("File list was null! SDL reports, \"{}\"", SDL_GetError());
  }
  else if (file[0] == nullptr)
  {
    log::debug("Canceled file dialog");
  }
  else
  {
    log::debug("Selected file '{}'", file[0]);
    auto &handle_file = *static_cast<HandleFile *>(impl);
    handle_file(file[0]);
  }
}