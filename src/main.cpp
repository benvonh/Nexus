#include "dt/app/application.h"
#include "dt/logging.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

int main(int argc, char *argv[])
{
    std::cout << '\n';
    std::cout << "+--------------+\n";
    std::cout << "| Digital Twin |\n";
    std::cout << "+--------------+\n";

    dt::log::debug("Digital Twin v?");

    try
    {
        dt::Application app(argc, argv);

        app.main_loop();
    }
    catch (const std::exception &e)
    {
        dt::log::error("Uh-oh... Something went wrong!");

        std::cerr << "\n  From exception of typename <";
        std::cerr << typeid(e).name() << ">, \n\n";
        std::cerr << "    " << e.what() << '\n';
    }

    std::cout << "\nDone\n";
}