#include "filedialog.h"

#include "dt/logging.h"
#include "dt/exception.h"

dt::FileDialog::FileDialog(SDL_Window *window)
{
    CHECK(window != nullptr, std::invalid_argument);
    CHECK(S_Window == nullptr, std::logic_error);
    S_Window = window;
}

dt::FileDialog::~FileDialog() noexcept(false)
{
    CHECK(S_Window != nullptr, std::logic_error);
    S_Window = nullptr;
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

        Client::queue(std::move(wrapper));
    }
}