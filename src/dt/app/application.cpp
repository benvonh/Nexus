#include "application.h"

#include "dt/gui/window.h"
#include "dt/exception.h"
#include "dt/logging.h"

#include "rclcpp/rclcpp.hpp"

dt::Application::Application(int argc, char **argv)
{
    log::debug("Initializing ROS...");
    rclcpp::init(argc, argv);
}

dt::Application::~Application()
{
    log::debug("Shutting down ROS...");
    rclcpp::shutdown();
}

void dt::Application::main_loop()
{
    Window window;

    while (window)
    {
        try
        {
            window.render_frame();
            window.handle_input();
        }
        catch (const dt::viewable_exception &e)
        {
            window.show_exception(e);
        }
    }
}