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
    std::cout << "dt::bridge::Node\n";
    auto node = std::make_shared<dt::bridge::Node>();

    std::cout << "rclcpp::executors::MultiThreadedExecutor\n";
    rclcpp::executors::MultiThreadedExecutor executor;

    std::cout << "rclcpp::executors::MultiThreadedExecutor::add_node\n";
    executor.add_node(node);

    std::cout << "rclcpp::executors::MultiThreadedExecutor::spin\n";
    executor.spin();
  }
  catch (const std::exception &e)
  {
    std::cerr << "\n\tException in ROS thread: "
              << "\n\t\t" << e.what() << std::endl;
  }
  std::cout << "Join\n";
}

int main(int argc, char *argv[])
{
  try
  {
    std::cout << "rclcpp::init\n";
    rclcpp::init(argc, argv);

    std::cout << "dt::view::Window\n";
    dt::view::Window window;

    std::cout << "std::jthread\n";
    std::jthread thread(ros_entry);

    std::cout << "dt::view::Window::operator()\n";
    window();

    std::cout << "rclcpp::shutdown\n";
    rclcpp::shutdown();
  }
  catch (const std::exception &e)
  {
    std::cerr << "\n\tException in main thread:"
              << "\n\t\t" << e.what() << std::endl;
  }
  std::cout << "Done\n";
}