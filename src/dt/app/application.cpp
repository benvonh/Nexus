#include "application.h"

#include "dt/core/filedialog.h"
#include "dt/core/window.h"
#include "dt/event/client.h"
#include "dt/exception.h"
#include "dt/logging.h"
#include "dt/render/render.h"
#include "dt/ui/log_history.h"
#include "dt/ui/menu_bar.h"
#include "dt/ui/scene_hierarchy.h"
#include "dt/ui/viewports.h"

#include "rclcpp/rclcpp.hpp"

dt::Application::Application(int argc, char **argv)
{
    log::debug("Intiailizing ROS client...");
    rclcpp::init(argc, argv);
}

dt::Application::~Application()
{
    log::debug("Shutting down ROS client...");
    rclcpp::shutdown();
}

void dt::Application::spin_thread()
{
    // _Thread = std::jthread(&dt::Application::_Exec_ROS, this);
}

void dt::Application::main_loop()
{
    Window window;
    Viewports viewports;
    FileDialog fileDialog(*window);

    while (window)
    {
        try
        {
            window.start_frame();

            draw_menu_bar();
            draw_log_history();
            draw_scene_hierarchy();

            viewports.draw();

            window.finish_frame();
            window.process_events();

            Client::Dispatch();

            throw_from_ros();
        }
        catch (const Exception &e)
        {
            window.show_exception(e);
        }
    }
}

void dt::Application::exec_ros()
{
    // while (true)
    // {
    //     try
    //     {
    //         // auto node = std::make_shared<dt::bridge::Node>();
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
}

void dt::Application::throw_from_ros()
{
    std::lock_guard guard(M_Mutex);

    if (M_Exception == nullptr) [[likely]]
        return;

    std::exception_ptr e;
    std::swap(M_Exception, e);
    std::rethrow_exception(e);
}