#include "window.h"
#include "window_theme.h"

#include "nexus/core/world.h"
#include "nexus/entity/robot.h"
#include "nexus/event/event_client.h"
#include "nexus/event/viewport_capture_event.h"
#include "nexus/logging.h"
#include "nexus/view/panel/log_history.h"
#include "nexus/view/panel/menu_bar.h"

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_opengl3.h"

#define SDL_TRY(function)                                            \
    do                                                               \
    {                                                                \
        if (!(function)) [[unlikely]]                                \
            throw std::runtime_error(EXPLAIN("{}", SDL_GetError())); \
    } while (0)

template <bool CAPTURE>
void Nexus::Window::ViewportCaptureMode::set(SDL_Window *window)
{
    if constexpr (CAPTURE)
    {
        SDL_TRY(SDL_SetWindowRelativeMouseMode(window, true));
        ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse;
    }
    else
    {
        ImGuiIO &io = ImGui::GetIO();

        const float mouseX = io.DisplaySize.x / 2;
        const float mouseY = io.DisplaySize.y / 2;

        SDL_WarpMouseInWindow(window, mouseX, mouseY);
        SDL_TRY(SDL_SetWindowRelativeMouseMode(window, false));

        io.ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
    }
    m_Captured = CAPTURE;
    LOG_EVENT("Viewport capture = {}", CAPTURE);
}

template void Nexus::Window::ViewportCaptureMode::set<true>(SDL_Window *);
template void Nexus::Window::ViewportCaptureMode::set<false>(SDL_Window *);

Nexus::Window::Window()
{
    LOG_BASIC("Initializing SDL for OpenGL 4.6 compatibility");

    SDL_TRY(SDL_Init(SDL_INIT_VIDEO));
    SDL_TRY(SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24));
    SDL_TRY(SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8));
    SDL_TRY(SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1));
    SDL_TRY(SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4));
    SDL_TRY(SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6));
    SDL_TRY(SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                                SDL_GL_CONTEXT_PROFILE_COMPATIBILITY));

    constexpr auto flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
    constexpr auto center = SDL_WINDOWPOS_CENTERED;

    LOG_BASIC("Creating SDL window...");
    m_Window = SDL_CreateWindow("Nexus v0", 1280, 720, flags);

    SDL_TRY(m_Window != nullptr);
    SDL_TRY(SDL_SetWindowPosition(m_Window, center, center));

    LOG_BASIC("Creating OpenGL context...");
    m_Context = SDL_GL_CreateContext(m_Window);

    SDL_TRY(m_Context != nullptr);
    SDL_TRY(SDL_GL_MakeCurrent(m_Window, m_Context));
    SDL_TRY(SDL_GL_SetSwapInterval(1));

    _create_layer();

    EventClient::On<ViewportCaptureEvent>(
        [this](const ViewportCaptureEvent &e)
        {
            m_ViewportCapture.set<true>(m_Window);
        });

    m_FileDialog.reset(new FileDialog(m_Window));
    m_MultiViewport.start_engine();
}

Nexus::Window::~Window() noexcept(false)
{
    m_MultiViewport.stop_engine();

    _destroy_layer();

    SDL_TRY(SDL_GL_DestroyContext(m_Context));
    SDL_DestroyWindow(m_Window);
    SDL_Quit();
}

void Nexus::Window::show_exception(const Exception &e)
{
    m_ViewportCapture.set<false>(m_Window);

    LOG_ERROR("{}", e.what());

    const auto flag = SDL_MESSAGEBOX_ERROR;
    const char *title = "Oops! An error occurred...";
    SDL_TRY(SDL_ShowSimpleMessageBox(flag, title, e.what(), m_Window));

    // Reset the ImGui layer to fix invalid states
    _destroy_layer();
    _create_layer();
}

void Nexus::Window::render_frame()
{
    const ImGuiIO &io = ImGui::GetIO();

    // Frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    // Draw
    ImGui::DockSpaceOverViewport();
    ImGui::ShowDemoWindow(&m_ShowDemo);

    _draw_low_level();

    draw_menu_bar();
    draw_log_history();

    m_PrimProperty.draw();
    m_MultiViewport.draw();
    m_SceneHierarchy.draw();

    // Render
    ImGui::Render();
    glViewport(0, 0, io.DisplaySize.x, io.DisplaySize.y);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Swap
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();
    SDL_TRY(SDL_GL_MakeCurrent(m_Window, m_Context));
    SDL_TRY(SDL_GL_SwapWindow(m_Window));
}

void Nexus::Window::handle_events()
{
    ImGuiIO &io = ImGui::GetIO();

    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_EVENT_QUIT ||
            (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED &&
                event.window.windowID == SDL_GetWindowID(m_Window)))
        {
            m_Live = false;
            return;
        }

        ImGui_ImplSDL3_ProcessEvent(&event);
    }

    // NOTE(Ben): I admit defeat in trying to fix the bug
    // where the first mouse click (left as to capture viewport mode)
    // is not registered by SDL. A second click is needed for ImGUI to
    // react properly again.
    if (m_ViewportCapture)
    {
        // SDL_PumpEvents();
        Render &render = m_MultiViewport.get_active_render();

        float x, y;
        auto mouseState = SDL_GetRelativeMouseState(&x, &y);
        render.look(x, y, io.DeltaTime);

        if (mouseState & SDL_BUTTON_LEFT)
        {
            render.intersection();
        }

        const bool *keyboard = SDL_GetKeyboardState(nullptr);

        if (keyboard[SDL_SCANCODE_W])
        {
            render.move<Controller::Direction::FORWARD>(io.DeltaTime);
        }
        if (keyboard[SDL_SCANCODE_A])
        {
            render.move<Controller::Direction::LEFT>(io.DeltaTime);
        }
        // if (keyboard[SDL_SCANCODE_S])
        if (keyboard[SDL_SCANCODE_R])
        {
            render.move<Controller::Direction::BACKWARD>(io.DeltaTime);
        }
        // if (keyboard[SDL_SCANCODE_D])
        if (keyboard[SDL_SCANCODE_S])
        {
            render.move<Controller::Direction::RIGHT>(io.DeltaTime);
        }
        if (keyboard[SDL_SCANCODE_SPACE])
        {
            render.move<Controller::Direction::UP>(io.DeltaTime);
        }
        if (keyboard[SDL_SCANCODE_LSHIFT])
        {
            render.move<Controller::Direction::DOWN>(io.DeltaTime);
        }
        if (keyboard[SDL_SCANCODE_ESCAPE])
        {
            m_MultiViewport.no_capture();
            m_ViewportCapture.set<false>(m_Window);
        }
    }
}

void Nexus::Window::_create_layer()
{
    LOG_BASIC("Creating ImGui context...");

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ASSERT(ImGui_ImplOpenGL3_Init("#version 460 core"));
    ASSERT(ImGui_ImplSDL3_InitForOpenGL(m_Window, m_Context));

    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    // TODO
    io.Fonts->AddFontFromFileTTF(R"(C:\Windows\Fonts\CascadiaCode.ttf)", 16.f);
    io.IniFilename = R"(C:\pixi_ws\Nexus\imgui.ini)";

    LOG_BASIC("Updating ImGui theme...");

    ImGuiStyle &style = ImGui::GetStyle();
    ImVec4 *colors = style.Colors;
    WindowTheme::UpdateStyle(style);
    WindowTheme::UpdateColor(colors);
}

void Nexus::Window::_destroy_layer()
{
    LOG_BASIC("Destroying ImGui context...");
    ImGui_ImplSDL3_Shutdown();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();
}

void Nexus::Window::_draw_low_level()
{
    const ImGuiIO &io = ImGui::GetIO();

    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::Button("Test Add Entity"))
        {
            World::AddEntity<Robot>(this, R"(C:\pixi_ws\Nexus\assets\franka\franka.urdf)");
        }

        ImGui::EndMainMenuBar();
    }

    if (ImGui::Begin("Performance Metrics"))
    {
        static float dt = 0.f;
        static float fps = 0.f;
        static float elapsed = 1.f;

        if (elapsed < .1f)
        {
            elapsed += io.DeltaTime;
        }
        else
        {
            dt = io.DeltaTime * 1000.f;
            fps = io.Framerate;
            elapsed = 0.f;
        }

        ImGui::Text("FPS: %.1f (%.1f ms)", fps, dt);

        ImGui::SeparatorText("Swap Interval");

        if (ImGui::Button("Max"))
        {
            LOG_EVENT("Max frame rate");
            SDL_TRY(SDL_GL_SetSwapInterval(0));
        }
        ImGui::SameLine();
        if (ImGui::Button("VSync"))
        {
            LOG_EVENT("VSync On");
            SDL_TRY(SDL_GL_SetSwapInterval(1));
        }
        ImGui::SameLine();
        if (ImGui::Button("Adaptive"))
        {
            LOG_EVENT("Adaptive Sync On");
            SDL_TRY(SDL_GL_SetSwapInterval(-1));
        }
    }
    ImGui::End();
}