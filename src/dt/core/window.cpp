#include "window.h"
#include "window_theme.h"

#include "dt/event/keyboard_event.h"
#include "dt/event/mouse_event.h"
#include "dt/event/viewport_capture_event.h"
#include "dt/exception.h"
#include "dt/logging.h"

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_opengl3.h"

#define SDL_DO(fn)                                        \
    do                                                    \
    {                                                     \
        if (!(fn)) [[unlikely]]                           \
            throw std::runtime_error(ex(SDL_GetError())); \
    } while (0)

#define SDL_ASSERT(expr) SDL_DO(expr)

dt::Window::Window()
{
    SDL_DO(SDL_Init(SDL_INIT_VIDEO));
    SDL_DO(SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24));
    SDL_DO(SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8));
    SDL_DO(SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1));
    SDL_DO(SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4));
    SDL_DO(SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6));
    SDL_DO(SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                               SDL_GL_CONTEXT_PROFILE_COMPATIBILITY));

#ifdef _DEBUG
    const char *windowTitle = "Digital Twin (Debug)";
#else
    const char *windowTitle = "Digital Twin (Release)";
#endif

    constexpr unsigned windowFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
    M_Window = SDL_CreateWindow(windowTitle, 1280, 720, windowFlags);
    SDL_ASSERT(M_Window != nullptr);

    constexpr auto windowCenter = SDL_WINDOWPOS_CENTERED;
    SDL_DO(SDL_SetWindowPosition(M_Window, windowCenter, windowCenter));

    M_Context = SDL_GL_CreateContext(M_Window);
    SDL_ASSERT(M_Context != nullptr);
    SDL_DO(SDL_GL_MakeCurrent(M_Window, M_Context));
    SDL_DO(SDL_GL_SetSwapInterval(1));

    this->create_layer();
    this->on<ViewportCaptureEvent>(
        [this](const ViewportCaptureEvent &e)
        {
            log::debug("Viewport capture = {}", e.Capture);
            ImGuiIO &io = ImGui::GetIO();
            M_InViewport = e.Capture;

            if (e.Capture)
            {
                SDL_SetWindowRelativeMouseMode(M_Window, true);
                io.ConfigFlags |= ImGuiConfigFlags_NoMouse;
            }
            else
            {
                const float mouseX = io.DisplaySize.x / 2;
                const float mouseY = io.DisplaySize.y / 2;
                SDL_WarpMouseInWindow(M_Window, mouseX, mouseY);
                SDL_DO(SDL_SetWindowRelativeMouseMode(M_Window, false));
                io.ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
                // SDL_PumpEvents();
                // SDL_FlushEvents(SDL_EVENT_FIRST, SDL_EVENT_LAST);
            }
        });
}

dt::Window::~Window() noexcept(false)
{
    this->destroy_layer();
    SDL_DO(SDL_GL_DestroyContext(M_Context));
    SDL_DestroyWindow(M_Window);
    SDL_Quit();
}

void dt::Window::show_exception(const Exception &e)
{
    log::debug("Displaying exception in message box...");
    this->send<ViewportCaptureEvent>(false);
    const auto flag = SDL_MESSAGEBOX_ERROR;
    const char *title = "Oops! An error occurred...";
    SDL_DO(SDL_ShowSimpleMessageBox(flag, title, e.what(), M_Window));
    this->destroy_layer();
    this->create_layer();
}

void dt::Window::set_vsync(bool enable)
{
    log::event("VSync = {}", enable);
    // TODO: We can support adaptive VSync
    SDL_DO(SDL_GL_SetSwapInterval(enable ? 1 : 0));
}

void dt::Window::start_frame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL3_NewFrame();

    ImGui::NewFrame();
    ImGui::DockSpaceOverViewport();
    ImGui::ShowDemoWindow(&M_ShowDemo);
}

void dt::Window::finish_frame()
{
    ImGui::Render();

    const ImGuiIO &io = ImGui::GetIO();

    glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    SDL_DO(SDL_GL_SwapWindow(M_Window));
}

void dt::Window::process_events()
{
    ImGuiIO &io = ImGui::GetIO();

    SDL_Event event;

    if (M_InViewport)
    {
        // while (SDL_PollEvent(&event))
        // {
        //     ImGui_ImplSDL3_ProcessEvent(&event);
        // }

        float x, y;
        SDL_GetRelativeMouseState(&x, &y);
        this->send<MouseEvent>(x, y, io.DeltaTime);

        const bool *keyboard = SDL_GetKeyboardState(nullptr);
        this->send<KeyboardEvent>(keyboard, io.DeltaTime);
    }
    else
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT ||
                (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED &&
                 event.window.windowID == SDL_GetWindowID(M_Window)))
            {
                M_Live = false;
                return;
            }

            if (ImGui_ImplSDL3_ProcessEvent(&event))
                // continue;

                if (event.type == SDL_EVENT_KEY_DOWN &&
                    event.key.scancode == SDL_SCANCODE_W)
                {
                    log::event("W");
                }
        }
    }
}

void dt::Window::create_layer()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    CHECK(ImGui_ImplOpenGL3_Init("#version 460 core"), std::runtime_error);
    CHECK(ImGui_ImplSDL3_InitForOpenGL(M_Window, M_Context), std::runtime_error);

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

void dt::Window::destroy_layer()
{
    ImGui_ImplSDL3_Shutdown();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();
}