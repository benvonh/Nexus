#pragma once

namespace dt
{
    class Application
    {
    public:
        Application(int argc, char **argv);

        ~Application();

        void main_loop();
    };
}