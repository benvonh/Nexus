#include "dt/app.hpp"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

int main(int argc, char *argv[])
{
  // TODO: Nicer welcome banner
  std::cout << "+---------------------+\n";
  std::cout << "|  Digital Twin (v0)  |\n";
  std::cout << "+---------------------+\n";

  try
  {
    dt::App app(argc, argv);

    app.SpinROS();
    app.MainLoop();
  }
  catch (const std::exception &e)
  {
    // TODO: Someday, find a nice exception message
    std::cerr << "\nUh-oh... An exception has been caught!\n";
    std::cerr << "\n  From exception of type " << typeid(e).name();
    std::cerr << "\n    " << e.what() << std::endl;
  }
  std::cout << "Done\n";
}