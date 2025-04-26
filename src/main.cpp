#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "dt/bridge/node.hpp"
#include "dt/view/render.hpp"
#include "dt/view/window.hpp"
#include "rclcpp/rclcpp.hpp"

#include <thread>
#include <iostream>
#include <stacktrace>


void ros_entry()
{
  rclcpp::spin(std::make_shared<dt::bridge::Node>());
}


int main(int argc, char *argv[])
{
  rclcpp::init(argc, argv);

  std::jthread thread(ros_entry);

  dt::view::Window window;
  dt::view::Render render;

  // TODO: Remove later
  dt::scene::Manager::DebugStage();

  try
  {
    window(render);
  }
  catch (const std::exception &e)
  {
    std::cerr << e.what() << std::endl;
    std::cerr << std::stacktrace::current() << std::endl;
  }

  rclcpp::shutdown();
}