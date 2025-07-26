#include "application.h"

#include "nexus/view/window.h"
#include "nexus/exception.h"

Nexus::Application::Application(int argc, char **argv)
{
    LOG_BASIC("Initializing ROS...");
    rclcpp::init(argc, argv);
}

Nexus::Application::~Application()
{
    LOG_BASIC("Shutting down ROS...");
    rclcpp::shutdown();
}

void Nexus::Application::spin_thread()
{
    // _Thread = std::jthread(&Nexus::Application::_Exec_ROS, this);
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

            Client::Dispatch();

            // this->throw_from_ros();
        }
        catch (const Exception &e)
        {
            window.show_exception(e);
        }
    }
}

// void Nexus::Application::exec_ros()
// {
// while (true)
// {
//     try
//     {
//         // auto node = std::make_shared<Nexus::bridge::Node>();
//         // auto exec = rclcpp::executors::MultiThreadedExecutor();
//         // exec.add_node(node);
//         // exec.spin();
//         break;
//     }
//     catch (const std::exception &e)
//     {
//         while (true)
//         {
//             std::this_thread::yield();
//             std::lock_guard guard(_Mutex);

//             if (_Exception != nullptr)
//             {
//                 _Exception = std::make_exception_ptr(e);
//                 break;
//             }
//         }
//     }
// }
// }

// void Nexus::Application::throw_from_ros()
// {
//     std::lock_guard guard(M_Mutex);

//     if (M_Exception == nullptr) [[likely]]
//         return;

//     std::exception_ptr e;
//     std::swap(M_Exception, e);
//     std::rethrow_exception(e);
// }