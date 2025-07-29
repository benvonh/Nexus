#include "application.h"

#include "nexus/core/world.h"
#include "nexus/view/window.h"

#include "rclcpp/utilities.hpp"

Nexus::Application::Application(int argc, char **argv)
{
    LOG_BASIC("Initializing ROS with {} argument{}",
              argc, argc == 1 ? '\0' : 's');

    for (int i = 0; i < argc; ++i)
    {
        LOG_BASIC("  [{}] = {}", i, argv[i]);
    }

    rclcpp::init(argc, argv);
}

Nexus::Application::~Application()
{
    LOG_BASIC("Shutting down ROS...");
    rclcpp::shutdown();
}

void Nexus::Application::world_core()
{
    m_Thread = std::jthread(
        [this]()
        {
            while (true)
            {
                try
                {
                    World::Executor executor;
                    World::SetExecutor(&executor);
                    executor.spin();
                    break;
                }
                catch (const std::exception &e)
                {
                    _prepare_to_throw(e);
                }
            }
        });
}

void Nexus::Application::main_loop()
{
    Window window;

    while (window)
    {
        try
        {
            window.render_frame();
            window.handle_events();

            EventClient::Dispatch();

            _throw_from_thread();
        }
        catch (const Exception &e)
        {
            window.show_exception(e);
        }
    }
}

void Nexus::Application::_prepare_to_throw(const std::exception &e)
{
    while (true)
    {
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(1s);
        std::lock_guard guard(m_Mutex);

        if (m_Exception == nullptr)
        {
            m_Exception = std::make_exception_ptr(e);
            LOG_ALERT("{} incoming", typeid(e).name());
            return;
        }
    }
}

void Nexus::Application::_throw_from_thread()
{
    std::lock_guard guard(m_Mutex);

    if (!m_Exception) [[likely]]
        return;

    std::exception_ptr e;
    std::swap(m_Exception, e);
    std::rethrow_exception(e);
}