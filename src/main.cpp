#include "dt/app.hpp"
#include "rclcpp/rclcpp.hpp"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

int main(int argc, char *argv[])
{
  std::cout << "+----------------+\n";
  std::cout << "|  Digital Twin  |\n";
  std::cout << "+----------------+\n";

  try
  {
    rclcpp::init(argc, argv);

    dt::App app;

    app.StartROS();
    app.MainLoop();

    rclcpp::shutdown();
  }
  catch (const std::exception &e)
  {
    std::cerr << "\nUh-oh... An exception has been caught!\n";
    std::cerr << "\n  From exception of type " << typeid(e).name();
    std::cerr << "\n    " << e.what() << std::endl;
  }
}