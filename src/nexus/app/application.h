#pragma once

#include "nexus/logging.h"

#include "rclcpp/rclcpp.hpp"

// #include <mutex>
// #include <thread>
// #include <exception>

namespace Nexus
{
    class Application : Logger<"Application">
    {
    public:
        Application(int argc, char **argv);

        ~Application();

        void spin_thread();

        void main_loop();

    private:
        // std::mutex M_Mutex;
        // std::jthread M_Thread;
        // std::exception_ptr M_Exception;
    };
}