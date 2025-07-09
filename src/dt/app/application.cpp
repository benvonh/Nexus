#include "application.h"

// #include "dt/core/event.h"
#include "dt/logging.h"
#include "dt/exception.h"
#include "dt/core/window.h"
#include "dt/render/render.h"
#include "dt/ui/log_output.h"
#include "dt/ui/scene_hierarchy.h"

#include "rclcpp/rclcpp.hpp"

dt::Application::Application(int argc, char **argv)
{
    rclcpp::init(argc, argv);
}

dt::Application::~Application()
{
    rclcpp::shutdown();
}

void dt::Application::SpinThread()
{
    // _Thread = std::jthread(&dt::Application::_Exec_ROS, this);
}

void dt::Application::MainLoop()
{
    Window window;
    Render render;

    while (window)
    {
        try
        {
            window.StartFrame();

            draw_log_output();
            draw_scene_hierarchy();

            Parameter::Draw();
            Controller::Draw();

            render.Draw();

            window.FinishFrame();
            window.HandleEvents();

            _Throw_from_ROS();
        }
        catch (const __exception__ &e)
        {
            window.ShowException(e);
        }
    }
}

void dt::Application::_Exec_ROS()
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

void dt::Application::_Throw_from_ROS()
{
    std::lock_guard guard(_Mutex);

    if (_Exception == nullptr) [[likely]]
        return;

    std::exception_ptr e;
    std::swap(_Exception, e);
    std::rethrow_exception(e);
}