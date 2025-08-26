#pragma once

#include <exception>
#include <mutex>
#include <thread>

namespace Nexus
{
    class Application final
    {
    public:
        Application(int argc, char **argv);

        ~Application();

        void world_core();
        void main_loop();

    private:
        void _prepare_to_throw(const std::exception &);
        void _throw_from_thread();

    private:
        std::mutex m_Mutex;
        std::jthread m_Thread;
        std::exception_ptr m_Exception;
    };
}