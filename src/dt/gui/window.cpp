#include "window.h"
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl3.h"
#include "../exception.h"
#include "../logging.h"

#define SDL_ERROR std::runtime_error(dt::exception(SDL_GetError()))

/*============================================================================*/
dt::Window::Window()
{
    if (!SDL_Init(SDL_INIT_VIDEO))
        throw SDL_ERROR;

    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);

    constexpr unsigned windowFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
    _Window = SDL_CreateWindow("Digital Twin", 1280, 720, windowFlags);

    if (_Window == nullptr)
        throw SDL_ERROR;

    SDL_SetWindowPosition(_Window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    _Context = SDL_GL_CreateContext(_Window);

    if (_Context == nullptr)
        throw SDL_ERROR;

    SDL_GL_MakeCurrent(_Window, _Context);
    SDL_GL_SetSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::GetStyle().WindowRounding = 8.f;
    ImGui_ImplSDL3_InitForOpenGL(_Window, _Context);
    ImGui_ImplOpenGL3_Init("#version 460 core");

    _Render.reset();
}

/*============================================================================*/
dt::Window::~Window()
{
    _Render.reset();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DestroyContext(_Context);
    SDL_DestroyWindow(_Window);
    SDL_Quit();
}

/*============================================================================*/
void dt::Window::show_exception(const dt::viewable_exception &e)
{
    if (!SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Exception", e, _Window))
        throw SDL_ERROR;
}

/*============================================================================*/
void dt::Window::render_frame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
    ImGui::DockSpaceOverViewport();
    ImGuiIO &io = ImGui::GetIO();

    _Controller.draw();
    _Parameter.draw();
    _Render.draw();

    ImGui::ShowDemoWindow(&_ShowDemo);
    ImGui::Render();
    glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(_Window);
}

/*============================================================================*/
void dt::Window::handle_input()
{
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        ImGui_ImplSDL3_ProcessEvent(&event);

        if (event.type == SDL_EVENT_QUIT ||
            (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED &&
             event.window.windowID == SDL_GetWindowID(_Window)))
        {
            _IsLive = false;
        }

        if (event.type == SDL_EVENT_KEY_DOWN &&
            event.key.scancode == SDL_SCANCODE_Q)
        {
            SDL_Event quitEvent;
            quitEvent.type = SDL_EVENT_QUIT;
            SDL_PushEvent(&quitEvent);
        }
    }
}