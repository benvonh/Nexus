#include "application.h"

#include "dt/exception.h"
#include "dt/logging.h"

#include "rclcpp/rclcpp.hpp"

dt::Application::Application(int argc, char **argv)
{
    log::debug("Initializing ROS2 client library...");
    rclcpp::init(argc, argv);
}

dt::Application::~Application()
{
    log::debug("Shutting down ROS2 client library...");
    rclcpp::shutdown();
}

void dt::Application::main_loop()
{
    while (_Window)
    {
        try
        {
            _Window.render_frame();
            _Window.handle_input();
        }
        catch (const dt::viewable_exception &e)
        {
            _Window.show_exception(e);
        }
    }
}