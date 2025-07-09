#include "window.h"
#include "window_theme.h"

#include "dt/logging.h"
#include "dt/exception.h"

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_opengl3.h"

#define SDL_INVOKE(__fn__) \
    DT_INVOKE(__fn__, std::runtime_error, SDL_GetError())

#define IMGUI_INVOKE(__fn__) \
    DT_INVOKE(__fn__, std::runtime_error, "ImGUI failure!")

// We want to specify why an expression fails
// with the SDL error message so the macro
// is the same.
#define SDL_ASSERT SDL_INVOKE

dt::Window::Window()
{
    SDL_INVOKE(SDL_Init(SDL_INIT_VIDEO));
    SDL_INVOKE(SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24));
    SDL_INVOKE(SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8));
    SDL_INVOKE(SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1));
    SDL_INVOKE(SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4));
    SDL_INVOKE(SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6));
    SDL_INVOKE(SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                                   SDL_GL_CONTEXT_PROFILE_COMPATIBILITY));

#ifdef _DEBUG
    const char *windowTitle = "Digital Twin (Debug)";
#else
    const char *windowTitle = "Digital Twin (Release)";
#endif

    constexpr unsigned windowFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
    _Window = SDL_CreateWindow(windowTitle, 1280, 720, windowFlags);
    SDL_ASSERT(_Window != nullptr);

    constexpr auto windowCenter = SDL_WINDOWPOS_CENTERED;
    SDL_INVOKE(SDL_SetWindowPosition(_Window, windowCenter, windowCenter));

    _Context = SDL_GL_CreateContext(_Window);
    SDL_ASSERT(_Context != nullptr);
    SDL_INVOKE(SDL_GL_MakeCurrent(_Window, _Context));
    SDL_INVOKE(SDL_GL_SetSwapInterval(1));
    __create_layer();
}

dt::Window::~Window() noexcept(false)
{
    __destroy_layer();
    SDL_INVOKE(SDL_GL_DestroyContext(_Context));
    SDL_DestroyWindow(_Window);
    SDL_Quit();
}

void dt::Window::ShowException(const dt::__exception__ &e)
{
    const auto flag = SDL_MESSAGEBOX_ERROR;
    const char *title = "Oops! An error occurred...";
    SDL_INVOKE(SDL_ShowSimpleMessageBox(flag, title, e.what(), _Window));
    __destroy_layer();
    __create_layer();
}

void dt::Window::SetVSync(bool enable)
{
    // TODO: We can support adaptive VSync
    SDL_INVOKE(SDL_GL_SetSwapInterval(enable ? 1 : 0));
}

void dt::Window::StartFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL3_NewFrame();

    ImGui::NewFrame();
    ImGui::DockSpaceOverViewport();
    ImGui::ShowDemoWindow(&_Show_demo);
}

void dt::Window::FinishFrame()
{
    ImGui::Render();

    const ImGuiIO &io = ImGui::GetIO();

    glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    SDL_INVOKE(SDL_GL_SwapWindow(_Window));
}

void dt::Window::HandleEvents()
{
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        if (ImGui_ImplSDL3_ProcessEvent(&event))
            continue;

        if (event.type == SDL_EVENT_QUIT ||
            (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED &&
             event.window.windowID == SDL_GetWindowID(_Window)))
        {
            _Live = false;
            return;
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

bool dt::Window::ControlRender(Render &render)
{
    ImGuiIO &io = ImGui::GetIO();

    float x, y;
    const SDL_MouseButtonFlags state = SDL_GetRelativeMouseState(&x, &y);
    render.Look(x, y, io.DeltaTime);

    const bool *keyboard = SDL_GetKeyboardState(nullptr);

    if (keyboard[SDL_SCANCODE_W])
    {
        render.Move<Controller::Direction::FORWARD>(io.DeltaTime);
    }
    if (keyboard[SDL_SCANCODE_A])
    {
        render.Move<Controller::Direction::LEFT>(io.DeltaTime);
    }
    // if (keyboard[SDL_SCANCODE_S])
    if (keyboard[SDL_SCANCODE_R])
    {
        render.Move<Controller::Direction::BACKWARD>(io.DeltaTime);
    }
    // if (keyboard[SDL_SCANCODE_D])
    if (keyboard[SDL_SCANCODE_R])
    {
        render.Move<Controller::Direction::RIGHT>(io.DeltaTime);
    }
    if (keyboard[SDL_SCANCODE_SPACE])
    {
        render.Move<Controller::Direction::UP>(io.DeltaTime);
    }
    if (keyboard[SDL_SCANCODE_LSHIFT])
    {
        render.Move<Controller::Direction::DOWN>(io.DeltaTime);
    }
    if (keyboard[SDL_SCANCODE_ESCAPE])
    {
        const float mouseX = io.DisplaySize.x / 2;
        const float mouseY = io.DisplaySize.y / 2;
        SDL_WarpMouseInWindow(_Window, mouseX, mouseY);
        SDL_SetWindowRelativeMouseMode(_Window, false);
        io.ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
        return false;
    }
    return true;
}

void dt::Window::__create_layer()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    IMGUI_INVOKE(ImGui_ImplOpenGL3_Init("#version 460 core"));
    IMGUI_INVOKE(ImGui_ImplSDL3_InitForOpenGL(_Window, _Context));

    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    // TODO: Investigate bug
    // io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    // TODO
    io.Fonts->AddFontFromFileTTF(R"(C:\Windows\Fonts\CascadiaCode.ttf)", 16.f);
    io.IniFilename = R"(C:\pixi_ws\DigitalTwin\imgui.ini)";

    ImGuiStyle &style = ImGui::GetStyle();
    ImVec4 *colors = style.Colors;
    WindowTheme::UpdateStyle(style);
    WindowTheme::UpdateColor(colors);
}

void dt::Window::__destroy_layer()
{
    ImGui_ImplSDL3_Shutdown();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();
}