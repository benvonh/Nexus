#include "window.h"

#include "dt/gui/sub/controller.h"
#include "dt/gui/sub/parameter.h"
#include "dt/gui/filedialog.h"
#include "dt/exception.h"
#include "dt/logging.h"

#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl3.h"
#include "imgui.h"

#define SDL_ERROR std::runtime_error(dt::exception(SDL_GetError()))

/*============================================================================*/
dt::Window::Window()
{
    log::debug("Initializing SDL3...");

    if (!SDL_Init(SDL_INIT_VIDEO))
        throw SDL_ERROR;

    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);

    log::debug("Creating SDL3 window...");

    constexpr unsigned windowFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
    _Window = SDL_CreateWindow("Digital Twin", 1280, 720, windowFlags);

    if (_Window == nullptr)
        throw SDL_ERROR;

    log::debug("Creating OpenGL context...");

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
    io.Fonts->AddFontFromFileTTF(R"(C:\Windows\Fonts\CascadiaCode.ttf)", 14.f);

    ImGui::GetStyle().WindowRounding = 8.f;
    ImGui_ImplSDL3_InitForOpenGL(_Window, _Context);
    ImGui_ImplOpenGL3_Init("#version 460 core");

    // TODO: Do my own style :D
    // https://github.com/shivang51/bess/blob/72755949d1c9ec3ed44f81670cd77b53ada39ad9/Bess/src/settings/themes.cpp#L39
    ImGuiStyle &style = ImGui::GetStyle();
    ImVec4 *colors = style.Colors;

    // Base color scheme
    colors[ImGuiCol_Text] = ImVec4(0.92f, 0.92f, 0.92f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.10f, 0.10f, 0.11f, 0.94f);
    colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.21f, 0.22f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.25f, 0.26f, 0.27f, 1.00f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.18f, 0.19f, 0.20f, 1.00f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.15f, 0.15f, 0.16f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.15f, 0.15f, 0.16f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.15f, 0.15f, 0.16f, 1.00f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.20f, 0.20f, 0.21f, 1.00f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.20f, 0.21f, 0.22f, 1.00f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.28f, 0.28f, 0.29f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.33f, 0.34f, 0.35f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.40f, 0.40f, 0.41f, 1.00f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.76f, 0.76f, 0.76f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.37f, 0.61f, 1.00f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.20f, 0.25f, 0.30f, 1.00f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.30f, 0.35f, 0.40f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.25f, 0.30f, 0.35f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.25f, 0.25f, 0.25f, 0.80f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.30f, 0.30f, 0.30f, 0.80f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.35f, 0.35f, 0.35f, 0.80f);
    colors[ImGuiCol_Separator] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.33f, 0.67f, 1.00f, 1.00f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.33f, 0.67f, 1.00f, 1.00f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.37f, 0.61f, 1.00f, 1.00f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.37f, 0.61f, 1.00f, 1.00f);
    colors[ImGuiCol_Tab] = ImVec4(0.15f, 0.18f, 0.22f, 1.00f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.38f, 0.48f, 0.69f, 1.00f);
    colors[ImGuiCol_TabActive] = ImVec4(0.28f, 0.38f, 0.59f, 1.00f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.15f, 0.18f, 0.22f, 1.00f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.15f, 0.18f, 0.22f, 1.00f);
    colors[ImGuiCol_DockingPreview] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
    colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
    colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    colors[ImGuiCol_TableHeaderBg] = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
    colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
    colors[ImGuiCol_TableBorderLight] = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
    colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.28f, 0.56f, 1.00f, 0.35f);
    colors[ImGuiCol_DragDropTarget] = ImVec4(0.28f, 0.56f, 1.00f, 0.90f);
    colors[ImGuiCol_NavHighlight] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

    // Style adjustments
    style.WindowRounding = 5.3f;
    style.FrameRounding = 2.3f;
    style.ScrollbarRounding = 0;

    style.WindowTitleAlign = ImVec2(0.50f, 0.50f);
    style.WindowPadding = ImVec2(8.0f, 8.0f);
    style.FramePadding = ImVec2(5.0f, 5.0f);
    style.ItemSpacing = ImVec2(6.0f, 6.0f);
    style.ItemInnerSpacing = ImVec2(6.0f, 6.0f);
    style.IndentSpacing = 25.0f;

    for (auto &render : _Renders)
        render.reset();

    FileDialog::SetWindow(_Window);
}

/*============================================================================*/
dt::Window::~Window()
{
    for (auto &render : _Renders)
        render.reset();

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
    log::debug("Showing exception message...");

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

    __draw_menu();

    Render::CachePaths();
    Parameter::Draw();
    Controller::Draw();

    for (int i = 0; i < _Render_count; ++i)
    {
        if (_Renders[i].draw())
        {
            _Render_active = i;
            _Renders[i].enable_free_camera();
            SDL_SetWindowRelativeMouseMode(_Window, true);
            io.ConfigFlags |= ImGuiConfigFlags_NoMouse;
        }
    }

    ImGui::ShowDemoWindow(&_Show_demo);
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
    ImGuiIO &io = ImGui::GetIO();
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        ImGui_ImplSDL3_ProcessEvent(&event);

        if (event.type == SDL_EVENT_QUIT ||
            (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED &&
             event.window.windowID == SDL_GetWindowID(_Window)))
        {
            _Live = false;
        }

        if (event.type == SDL_EVENT_KEY_DOWN && event.key.scancode == SDL_SCANCODE_Q)
        {
            SDL_Event quitEvent;
            quitEvent.type = SDL_EVENT_QUIT;
            SDL_PushEvent(&quitEvent);
        }
    }

    if (_Render_active >= 0)
    {
        auto &render = _Renders[_Render_active];

        float x, y;
        const SDL_MouseButtonFlags state = SDL_GetRelativeMouseState(&x, &y);
        render.look(x, y, io.DeltaTime);

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
            const float mouseX = io.DisplaySize.x / 2;
            const float mouseY = io.DisplaySize.y / 2;
            SDL_WarpMouseInWindow(_Window, mouseX, mouseY);
            SDL_SetWindowRelativeMouseMode(_Window, false);
            io.ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
            _Render_active = -1;
        }
    }
}

/*============================================================================*/
void dt::Window::__draw_menu()
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Rendering"))
        {
            ImGui::Text("Active Renders: %d/%d", _Render_count, _Renders.size());

            if (ImGui::Button("Add Render Viewport"))
            {
                if (_Render_count < _Renders.size())
                {
                    _Render_count++;
                    _Renders[_Render_count - 1].reset();
                }
            }
            if (ImGui::Button("Remove Render Viewport"))
            {
                if (_Render_count > 1)
                {
                    _Render_count--;
                }
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
    else
        throw exception("Unexpected programming error");
}