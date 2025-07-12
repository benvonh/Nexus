//
//    Copyright 2025 Benjamin von Snarski
//
//    Licensed under the Apache License, Version 2.0 (the "License");
//    you may not use this file except in compliance with the License.
//    You may obtain a copy of the License at
//
//        http://www.apache.org/licenses/LICENSE-2.0
//
//    Unless required by applicable law or agreed to in writing, software
//    distributed under the License is distributed on an "AS IS" BASIS,
//    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//    See the License for the specific language governing permissions and
//    limitations under the License.
//
#include "dt/app/application.h"
#include "dt/logging.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

int main(int argc, char *argv[])
{
    dt::log::debug("Welcome to Digital Twin v0.1");

#ifdef _DEBUG
    dt::Application app(argc, argv);

    app.main_loop();
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