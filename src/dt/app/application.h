#pragma once

#include "dt/event/event.h"

#include <mutex>
#include <thread>
#include <exception>

namespace dt
{
    class Application
    {
    public:
        Application(int argc, char **argv);

        ~Application();

        void spin_thread();

        void main_loop();

    private:
        void exec_ros();
        void throw_from_ros();

        std::mutex M_Mutex;
        std::jthread M_Thread;
        std::exception_ptr M_Exception;
    };
}