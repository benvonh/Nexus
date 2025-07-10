#include "filedialog.h"

#include "dt/logging.h"
#include "dt/exception.h"

dt::FileDialog::FileDialog(SDL_Window *window)
{
    DT_THROW_IF(window == nullptr, std::invalid_argument);
    DT_THROW_IF(S__Window != nullptr, std::logic_error);
    S__Window = window;
}

dt::FileDialog::~FileDialog() noexcept(false)
{
    DT_THROW_IF(S__Window == nullptr, std::logic_error);
    S__Window = nullptr;
}

void dt::FileDialog::callback(void *userdata, const char *const *filelist, int filter) noexcept
{
    auto *fn = reinterpret_cast<std::function<void(std::string, int)> *>(userdata);

    if (filelist == nullptr)
    {
        dt::log::error("File list was null! SDL reports, \"{}\"", SDL_GetError());
        delete fn;
    }

    else if (filelist[0] == nullptr)
    {
        dt::log::debug("Canceled file dialog");
        delete fn;
    }
    else
    {
        std::string path(filelist[0]);

        auto wrapper = [fn, path, filter]()
        {
            (*fn)(path, filter);
            delete fn;
        };

        Client::Queue(std::move(wrapper));
    }
}