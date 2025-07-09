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

        void SpinThread();

        void MainLoop();

    private:
        void _Exec_ROS();
        void _Throw_from_ROS();

        std::mutex _Mutex;
        std::jthread _Thread;
        std::exception_ptr _Exception;
    };
}