#pragma once

#include "dt/gui/window.h"

namespace dt
{
    class Application
    {
    public:
        Application(int argc, char **argv);

        ~Application();

        void main_loop();

    private:
        Window _Window;
    };
}