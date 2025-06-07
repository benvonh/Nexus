#include "dt/base.hpp"
#include "dt/bridge/node.hpp"
#include "dt/view/window.hpp"
#include "rclcpp/rclcpp.hpp"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <iostream>
#include <thread>

void ros_entry()
{
  try
  {
    dt::log::debug("dt::bridge::Node()");
    auto node = std::make_shared<dt::bridge::Node>();

    dt::log::debug("rclcpp::executors::MultiThreadedExecutor()");
    rclcpp::executors::MultiThreadedExecutor executor;

    dt::log::debug("rclcpp::executors::MultiThreadedExecutor::add_node(node)");
    executor.add_node(node);

    dt::log::debug("rclcpp::executors::MultiThreadedExecutor::spin()");
    executor.spin();
  }
  catch (const std::exception &e)
  {
    std::cerr << "\n\tException in ROS thread:"
              << "\n\t\t" << e.what() << std::endl;
  }
  std::cout << "Join\n";
}

int main(int argc, char *argv[])
{
  try
  {
    dt::log::debug("rclcpp::init(argc, argv)");
    rclcpp::init(argc, argv);

    dt::log::debug("std::jthread(ros_entry)");
    std::jthread thread(ros_entry);

    dt::log::debug("dt::view::Window()");
    dt::view::Window window;

    dt::log::debug("dt::view::Window::operator()");
    window();

    dt::log::debug("rclcpp::shutdown()");
    rclcpp::shutdown();
  }
  catch (const std::exception &e)
  {
    std::cerr << "\n\tException in main thread:"
              << "\n\t\t" << e.what() << std::endl;
  }
  std::cout << "Done\n";
}