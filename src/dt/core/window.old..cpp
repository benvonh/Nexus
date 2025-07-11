#include "window.h"

#include "dt/gui/sub/controller.h"
#include "dt/gui/sub/parameter.h"
#include "dt/gui/file_dialog.h"
#include "dt/usd/world.h"
#include "dt/exception.h"
#include "dt/logging.h"

#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl3.h"
#include "imgui.h"

#define SDL_ERROR std::runtime_error(dt::exception(SDL_GetError()))

/*============================================================================*/
void dt::Window::FileHandler::invoke(const char *path, int _)
{
    FileHandler::Path = path;
}

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

#ifdef _DEBUG
    const char *windowTitle = "Digital Twin (Debug)";
#else
    const char *windowTitle = "Digital Twin (Release)";
#endif
    constexpr unsigned windowFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
    M_Window = SDL_CreateWindow(windowTitle, 1280, 720, windowFlags);

    if (M_Window == nullptr)
        throw SDL_ERROR;

    log::debug("Creating OpenGL context...");

    SDL_SetWindowPosition(M_Window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    M_Context = SDL_GL_CreateContext(M_Window);

    if (M_Context == nullptr)
        throw SDL_ERROR;

    SDL_GL_MakeCurrent(M_Window, M_Context);
    SDL_GL_SetSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplSDL3_InitForOpenGL(M_Window, M_Context);
    ImGui_ImplOpenGL3_Init("#version 460 core");

    for (auto &render : _Renders)
        render.reset();

    FileDialog::SetWindow(M_Window);
}

/*============================================================================*/
dt::Window::~Window()
{
    for (auto &render : _Renders)
        render.reset();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DestroyContext(M_Context);
    SDL_DestroyWindow(M_Window);
    SDL_Quit();
}

/*============================================================================*/
void dt::Window::show_exception(const dt::viewable_exception &e)
{
    log::debug("Showing exception message...");

    if (!SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Exception", e, M_Window))
        throw SDL_ERROR;
}

/*============================================================================*/
void dt::Window::render_frame()
{
RENDER_FRAME_START:
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
    ImGui::DockSpaceOverViewport();
    ImGuiIO &io = ImGui::GetIO();

    if (!__draw_menu())
        goto RENDER_FRAME_START;

    _Browser.draw();
    _Logs.draw();

    Parameter::Draw();
    Controller::Draw();

    Render::CachePaths();

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
bool dt::Window::__draw_menu()
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            // TODO: Should probably check if unsaved and prompt user
            if (ImGui::MenuItem("New", "Ctrl+N"))
            {
                FileHandler::Path.clear();
                FileDialog::Show<FileDialog::Mode::SAVE>(NewPtr<FileHandler>(), FileDialog::USD_FILTER);

                __modal_update_during_file_dialog();

                if (!FileHandler::Path.empty())
                {
                    for (auto &render : _Renders)
                        render.reset();

                    World::NewStage(FileHandler::Path);
                }

                return false;
            }
            if (ImGui::MenuItem("Open", "Ctrl+O"))
            {
                FileHandler::Path.clear();
                FileDialog::Show<FileDialog::Mode::OPEN>(NewPtr<FileHandler>(), FileDialog::USD_FILTER);

                __modal_update_during_file_dialog();

                if (!FileHandler::Path.empty())
                {
                    for (auto &render : _Renders)
                        render.reset();

                    World::OpenStage(FileHandler::Path);
                }

                return false;
            }
            if (ImGui::MenuItem("Save", "Ctrl+S"))
            {
                World::SaveStage();
            }
            if (ImGui::MenuItem("Export", "Ctrl+E"))
            {
                FileHandler::Path.clear();
                FileDialog::Show<FileDialog::Mode::SAVE>(NewPtr<FileHandler>(), FileDialog::USD_FILTER);

                __modal_update_during_file_dialog();

                if (!FileHandler::Path.empty())
                    World::ExportStage(FileHandler::Path);

                return false;
            }
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

    return true;
}

/*============================================================================*/
void dt::Window::__modal_update_during_file_dialog()
{
    // VERY DANGEROUS!
    //
    // We assume that the GUI is inaccessible
    // while a modal window is open (file dialog)
    //
    // We must NOT resume rendering where we left off;
    // Restart the frame then start rendering again!
    while (!FileDialog::Done)
    {
        this->render_frame();
        this->handle_input();
    }
}