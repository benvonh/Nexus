//
//  Copyright 2025 Benjamin von Snarski
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//
#include "nexus/app/application.h"
#include "nexus/logging.h"

int main(int argc, char **argv)
{
#ifdef _DEBUG
    LOG_BASIC_EntryPoint("Nexus Debug v0");

    Nexus::Application app(argc, argv);

    app.world_core();
    app.main_loop();
#else
    try
    {
        std::cout << "Copyright 2025 Benjamin von Snarski\n\n";

        LOG_BASIC("Nexus Release v0");

        Nexus::Application app(argc, argv);

        app.world_core();
        app.main_loop();
    }
    catch (const std::exception &e)
    {
        LOG_ERROR("An exception of type <{}> was thrown!", typeid(e).name());

        std::cerr << "\n    " << e.what() << "\n\n";

        return EXIT_FAILURE;
    }
#endif
    return EXIT_SUCCESS;
}

#ifdef _WIN32
#include <windows.h>
int WINAPI WinMain(HINSTANCE _0, HINSTANCE _1, LPSTR _2, int _3)
{
    return main(0, nullptr);
}
#endif