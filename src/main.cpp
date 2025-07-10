#include "dt/app/application.h"
#include "dt/logging.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

int main(int argc, char *argv[])
{
    dt::log::debug("Welcome to Digital Twin v0.1");

#ifdef _DEBUG
    dt::Application app(argc, argv);

    app.MainLoop();
#else
    try
    {
        dt::Application app(argc, argv);

        app.MainLoop();
    }
    catch (const std::exception &e)
    {
        dt::log::error("An exception of type <{}> was thrown!", typeid(e).name());

        std::cerr << "\n    " << e.what() << "\n\n";
    }
#endif

    dt::log::debug("Done");
}