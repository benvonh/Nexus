#include "dt/view/window.hpp"
#include "dt/scene/manager.hpp"

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_opengl3.h"
#include "stb_image_write.h"

#include "pxr/usd/usd/primRange.h"

#include <format>
#include <stacktrace>

#define THROW(msg)                \
  do { throw std::runtime_error(  \
    std::format(                  \
      "{}\n{}\n{}\n",             \
      std::string(msg),           \
      SDL_GetError(),             \
      std::stacktrace::current()) \
    );                            \
  } while (false)

namespace dt
{
namespace view
{
Window::Window()
{
  if (!SDL_Init(SDL_INIT_VIDEO))
    THROW("SDL Initialization");

  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                      SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);

  constexpr unsigned windowFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
  this->window = SDL_CreateWindow("Digital Twin", 1280, 720, windowFlags);

  if (this->window == nullptr)
    THROW("SDL Window Creation");

  SDL_SetWindowPosition(this->window,
                        SDL_WINDOWPOS_CENTERED,
                        SDL_WINDOWPOS_CENTERED);

  this->context = SDL_GL_CreateContext(this->window);

  if (this->context == nullptr)
    THROW("SDL Context Creation");

  SDL_GL_MakeCurrent(this->window, this->context);
  SDL_GL_SetSwapInterval(1);

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

  ImGui::GetStyle().WindowRounding = 8.f;
  ImGui_ImplSDL3_InitForOpenGL(this->window, this->context);
  ImGui_ImplOpenGL3_Init("#version 460 core");
}

Window::~Window()
{
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplSDL3_Shutdown();
  ImGui::DestroyContext();

  SDL_GL_DestroyContext(this->context);
  SDL_DestroyWindow(this->window);
  SDL_Quit();
}

void Window::operator()(Render &render)
{
  this->render_ptr = &render;
  
  while (this->operative) { this->Update(); }
}

void Window::Update()
{
  ImGuiIO &io = ImGui::GetIO();
  Render &render = *this->render_ptr;

  try
  { FRAME_START:
    ///////////////////////////////////////////////////////////////////////////
    // ImGUI Start
    ///////////////////////////////////////////////////////////////////////////
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
    ImGui::DockSpaceOverViewport();

    ///////////////////////////////////////////////////////////////////////////
    // Main Menu Bar
    ///////////////////////////////////////////////////////////////////////////
    if (ImGui::BeginMainMenuBar())
    {
      if (ImGui::BeginMenu("File"))
      {
        if (ImGui::MenuItem("New", "Ctrl+N"))
        {
          this->HandleFile<scene::Action::NEW>();
          goto FRAME_START;
        }
        if (ImGui::MenuItem("Open", "Ctrl+O"))
        {
          this->HandleFile<scene::Action::OPEN>();
          goto FRAME_START;
        }
        if (ImGui::MenuItem("Save", "Ctrl+S"))
        {
          this->HandleFile<scene::Action::SAVE>();
          goto FRAME_START;
        }
        if (ImGui::MenuItem("Export", "Ctrl+E"))
        {
          this->HandleFile<scene::Action::EXPORT>();
          goto FRAME_START;
        }
        ImGui::EndMenu();
      }
      ImGui::EndMainMenuBar();
    }

    ///////////////////////////////////////////////////////////////////////////
    // USD Render Parameter
    ///////////////////////////////////////////////////////////////////////////
    ImGui::Begin("Render Parameter");
    {
      render.params.frame = scene::Manager::GetTime();
      ImGui::Checkbox("Lighting", &render.params.enableLighting);
      ImGui::Checkbox("Scene Lights", &render.params.enableSceneLights);
      ImGui::Checkbox("Scene Materials", &render.params.enableSceneMaterials);
      ImGui::Checkbox("Show Guides", &render.params.showGuides);
      ImGui::Checkbox("Show Proxy", &render.params.showProxy);
      ImGui::Checkbox("Show Render", &render.params.showRender);
      ImGui::Checkbox("Force Refresh", &render.params.forceRefresh);
      ImGui::Checkbox("Sample Alpha to Coverage", &render.params.enableSampleAlphaToCoverage);
      ImGui::Checkbox("Gamma Correct Colors", &render.params.gammaCorrectColors);

      if (ImGui::Checkbox("Dome Light Camera Visibility", &render.domeLight))
      {
        render.UpdateDomeLight();
      }
      ImGui::SliderFloat("Complexity", &render.params.complexity, 1.f, 1.5f, "%.1f");
      ImGui::Combo("Draw Mode", (int*)&render.params.drawMode, render.DRAW_MODES, render.DRAW_MODES_SIZE);
      ImGui::Combo("Cull Style", (int*)&render.params.cullStyle, render.CULL_STYLES, render.CULL_STYLES_SIZE);
      ImGui::ColorEdit4("Clear Color", &render.params.clearColor[0]);
    }
    ImGui::End();

    ///////////////////////////////////////////////////////////////////////////
    // USD Render Viewport
    ///////////////////////////////////////////////////////////////////////////
    ImGui::Begin("USD Viewport");
    {
      if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(0))
      {
        // Click in USD world
        if (this->immersive)
        {

        }
        // Immerse in USD world
        else
        {
          SDL_SetWindowRelativeMouseMode(this->window, true);
          ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse;
          this->immersive = true;
        }
      }
      ImVec2 size = ImGui::GetContentRegionAvail();
      render.UpdateSize(size.x, size.y);
      render.UpdateCameraState(this->camera.data.GetFrustum());
      ImGui::Image(ImTextureID(render()), size, ImVec2(0, 1), ImVec2(1, 0));
    }
    ImGui::End();

    ///////////////////////////////////////////////////////////////////////////
    // USD Stage
    ///////////////////////////////////////////////////////////////////////////
    ImGui::Begin("Stage");
    {
      // TODO: Temporary
      ImGui::Text("FPS: %d", (int)(1.f / io.DeltaTime));

      auto entry = scene::Manager::GetStageEntry();
      auto range = entry.stage->Traverse();

      for (auto it = range.begin(); it != range.cend(); ++it)
      {
        if (it.IsPostVisit())
        {
          ImGui::TreePop();
        }
        else
        {
          if (it->GetChildren().empty())
          {
            ImGui::Text(it->GetTypeName().GetText());
          }
          else
          {
            ImGui::TreeNode(it->GetTypeName().GetText());
          }
        }
      }
    }
    ImGui::End();

    ///////////////////////////////////////////////////////////////////////////
    // ImGUI Finish
    ///////////////////////////////////////////////////////////////////////////
    bool yes = true;
    ImGui::ShowDemoWindow(&yes);
    ImGui::Render();
    glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(this->window);

    ///////////////////////////////////////////////////////////////////////////
    // User Input
    ///////////////////////////////////////////////////////////////////////////
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
      ImGui_ImplSDL3_ProcessEvent(&event);

      if (event.type == SDL_EVENT_QUIT ||
          (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED &&
           event.window.windowID == SDL_GetWindowID(this->window)))
      {
        this->operative = false;
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

    if (this->immersive)
    {
      float x, y;
      const SDL_MouseButtonFlags state = SDL_GetRelativeMouseState(&x, &y);
      this->camera.Look(x, y, io.DeltaTime);

      const bool *keyboard = SDL_GetKeyboardState(nullptr);

      if (keyboard[SDL_SCANCODE_W]) {
        this->camera.Move<Camera::Direction::FORWARD>(io.DeltaTime);
      }
      if (keyboard[SDL_SCANCODE_A]) {
        this->camera.Move<Camera::Direction::LEFT>(io.DeltaTime);
      }
      if (keyboard[SDL_SCANCODE_S]) {
        this->camera.Move<Camera::Direction::BACKWARD>(io.DeltaTime);
      }
      if (keyboard[SDL_SCANCODE_D]) {
        this->camera.Move<Camera::Direction::RIGHT>(io.DeltaTime);
      }
      if (keyboard[SDL_SCANCODE_SPACE]) {
        this->camera.Move<Camera::Direction::UP>(io.DeltaTime);
      }
      if (keyboard[SDL_SCANCODE_LSHIFT]) {
        this->camera.Move<Camera::Direction::DOWN>(io.DeltaTime);
      }
      if (keyboard[SDL_SCANCODE_ESCAPE])
      {
        const float mouseX = io.DisplaySize.x / 2;
        const float mouseY = io.DisplaySize.y / 2;
        SDL_WarpMouseInWindow(this->window, mouseX, mouseY);
        SDL_SetWindowRelativeMouseMode(this->window, false);
        io.ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
        this->immersive = false;
      }
    }
  }
  catch (const std::exception &e)
  {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, SDL_GetError());
    auto trace = std::stacktrace::current();
    auto string = std::to_string(trace);

    if (!MessageBox(SDL_MESSAGEBOX_ERROR, e.what(), string.c_str(), this->window))
      throw std::runtime_error(SDL_GetError());
  }
}
} // namespace view
} // namespace dt