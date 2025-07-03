#include "dt/app.hpp"
#include <print>


int main(int argc, char *argv[])
{
  // TODO: Nicer welcome banner
  std::cout << "+---------------------+\n";
  std::cout << "|  Digital Twin (v0)  |\n";
  std::cout << "+---------------------+\n";

  try
  {
    dt::App app(argc, argv);
    app.SpinThread();
    app.MainLoop();
  }
  catch (const std::exception &e)
  {
    // TODO: Someday, find a nice exception message
    std::cerr << "\nUh-oh... An exception has been caught!\n";
    std::cerr << "\n  From exception of type " << typeid(e).name();
    std::cerr << "\n    " << e.what() << std::endl;
  }
  std::println("Done");
}