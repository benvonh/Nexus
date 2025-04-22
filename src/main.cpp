#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
// #include "dt/bridge.hpp"
#include "dt/view/window.hpp"
#include "rclcpp/rclcpp.hpp"

#include <thread>
#include <iostream>
#include <stacktrace>


// void ros_entry()
// {
//   rclcpp::spin(std::make_shared<dt::Bridge>());
// }


int main(int argc, char *argv[])
{
  try
  {
    rclcpp::init(argc, argv);

    // if (!dt::SceneManager::OpenURDF("r2d2.urdf.xml"))
    //   return EXIT_FAILURE;

    // std::jthread thread(ros_entry);

    dt::view::Window window;
    
    while (window)
    {
      window.RenderFrame();
      window.HandleEvents();
    }

    rclcpp::shutdown();
  }
  catch (const std::exception &e)
  {
    std::cerr << "    what: " << e.what() << std::endl;
    std::cerr << std::stacktrace::current() << std::endl;
  }
  return 0;
}