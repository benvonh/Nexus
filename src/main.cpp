#include "dt/gui/window.h"
#include "rclcpp/rclcpp.hpp"

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);
    dt::Window window;

    while (window)
    {
        window.render_frame();
        window.handle_input();
    }
}