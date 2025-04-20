#include "dt/scene/manager.hpp"
// #include "dt/bridge.hpp"
#include "dt/window.hpp"
#include "rclcpp/rclcpp.hpp"

#include <thread>


// void ros_entry()
// {
//   rclcpp::spin(std::make_shared<dt::Bridge>());
// }


int main(int argc, char *argv[])
{
  rclcpp::init(argc, argv);

  // TODO: Eventually do from window
  if (!dt::scene::Manager::OpenFile("Sphere.usd"))
    return 1;

  // if (!dt::SceneManager::OpenURDF("r2d2.urdf.xml"))
  //   return EXIT_FAILURE;

  // std::jthread thread(ros_entry);

  dt::Window window;
  
  while (window)
  {
    window.RenderFrame();
    window.HandleEvents();
  }

  dt::scene::Manager::SaveFile();
  rclcpp::shutdown();
  return 0;
}