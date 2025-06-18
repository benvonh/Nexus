#include "dt/view/filedialog.hpp"
#include "dt/view/window.hpp"
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl3.h"
#include "pxr/usd/usd/primRange.h"

dt::view::Window::Window()
{
  if (!SDL_Init(SDL_INIT_VIDEO))
    throw exception("SDL Initialize: {}", SDL_GetError());

  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                      SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);

  constexpr unsigned windowFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
  _window = SDL_CreateWindow("Digital Twin", 1280, 720, windowFlags);

  if (_window == nullptr)
    throw exception("SDL Create Window: {}", SDL_GetError());

  SDL_SetWindowPosition(_window,
                        SDL_WINDOWPOS_CENTERED,
                        SDL_WINDOWPOS_CENTERED);

  this->context = SDL_GL_CreateContext(_window);

  if (this->context == nullptr)
    throw exception("SDL Create Context: {}", SDL_GetError());

  SDL_GL_MakeCurrent(_window, this->context);
  SDL_GL_SetSwapInterval(1);

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

  ImGui::GetStyle().WindowRounding = 8.f;
  ImGui_ImplSDL3_InitForOpenGL(_window, this->context);
  ImGui_ImplOpenGL3_Init("#version 460 core");

  _file_dialog.emplace(_window);
}

dt::view::Window::~Window()
{
  this->renders.clear();

  for (auto &plugin : this->plugins)
  {
    if (plugin.instance)
    {
      plugin.destroy(plugin.instance);
    }
    if (plugin.module)
    {
      FreeLibrary(plugin.module);
    }
  }

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplSDL3_Shutdown();
  ImGui::DestroyContext();

  SDL_GL_DestroyContext(this->context);
  SDL_DestroyWindow(_window);
  SDL_Quit();
}

void dt::view::Window::ShowException(const std::exception &e)
{
  if (!SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Exception", e.what(), _window))
    throw std::runtime_error(SDL_GetError());
}

bool dt::view::Window::Update()
{
  if (SDL_GetWindowFlags(_window) & SDL_WINDOW_MINIMIZED)
  {
    SDL_Log("Window is minimized (10 fps)");
    SDL_Delay(100);
  }

  /////////////////////////////////////////////////////////////////////////
  // ImGUI Start
  /////////////////////////////////////////////////////////////////////////
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplSDL3_NewFrame();
  ImGui::NewFrame();
  ImGui::DockSpaceOverViewport();
  ImGuiIO &io = ImGui::GetIO();

  /////////////////////////////////////////////////////////////////////////
  // Main Menu Bar
  /////////////////////////////////////////////////////////////////////////
  if (ImGui::BeginMainMenuBar())
  {
    if (ImGui::BeginMenu("File"))
    {
      if (ImGui::MenuItem("New", "Ctrl+N"))
      {
        FileDialog::Show<FileDialog::Mode::SAVE>(this->window);
        if (this->UpdateModal())
        {
          scene::Manager::HandleStage<scene::Action::NEW>(FileDialog::callback.path);
          this->DefaultRender();
        }
        return *this;
      }
      if (ImGui::MenuItem("Open", "Ctrl+O"))
      {
        FileDialog::Show<FileDialog::Mode::OPEN>(this->window);
        if (this->UpdateModal())
        {
          scene::Manager::HandleStage<scene::Action::OPEN>(FileDialog::callback.path);
          this->DefaultRender();
        }
        return *this;
      }
      if (ImGui::MenuItem("Save", "Ctrl+S"))
      {
        scene::Manager::HandleStage<scene::Action::SAVE>();
        return *this;
      }
      if (ImGui::MenuItem("Export", "Ctrl+E"))
      {
        FileDialog::Show<FileDialog::Mode::SAVE>(this->window);
        if (this->UpdateModal())
        {
          scene::Manager::HandleStage<scene::Action::OPEN>(FileDialog::callback.path);
        }
        return *this;
      }
      ImGui::Separator();

      if (ImGui::MenuItem("Import URDF"))
      {
        // TODO:
        scene::Manager::SetRobot("assets/franka/franka.urdf");
      }
      if (ImGui::MenuItem("Load Plugin"))
      {
        HMODULE mod = LoadLibraryW(LR"(C:\Projects\DigitalTwinPlugin\install\digital_twin_plugin\bin\plugin.dll)");
        if (!mod)
        {
          log::error("Failed to load plugin: {}", GetLastError());
          return true;
        }
        log::event("Plugin loaded successfully");
        auto create = reinterpret_cast<IPlugin *(*)()>(GetProcAddress(mod, "create_plugin"));
        auto destroy = reinterpret_cast<void (*)(IPlugin *)>(GetProcAddress(mod, "destroy_plugin"));
        if (!create || !destroy)
        {
          log::error("Failed to get plugin functions: {}", GetLastError());
          FreeLibrary(mod);
          return true;
        }
        IPlugin *inst = create();
        if (!inst)
        {
          log::error("Failed to create plugin instance");
          FreeLibrary(mod);
          return true;
        }
        log::event("Plugin instance created: {}", inst->GetName());
        this->plugins.push_back({mod, inst, destroy});
      }
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Render"))
    {
      if (ImGui::MenuItem("Add Viewport", "Ctrl+Shift+N"))
      {
        _render_group.AddRender();
      }
      if (ImGui::MenuItem("Remove Viewport", "Ctrl+Shift+R"))
      {
        log::alert("Remove viewport not implemented yet");
      }
      ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
  }
  else
    throw exception("Problem with main menu bar");

  /////////////////////////////////////////////////////////////////////////
  // USD Render Parameter
  /////////////////////////////////////////////////////////////////////////
  ImGui::Begin("Render Parameter");
  ImGui::Text("FPS: %d", (int)(1.f / io.DeltaTime));
  Parameter::Draw();
  Controller::Draw();
  ImGui::End();

  /////////////////////////////////////////////////////////////////////////
  // USD Render Viewport
  /////////////////////////////////////////////////////////////////////////
  {
    scene::StagePermit permit = scene::Manager::GetStagePermit();
    Render::CachePaths(permit.stage);

    for (int i = 0; i < this->renders.size(); ++i)
    {
      Render &render = this->renders[i];
      ImGui::Begin(render.name.c_str(), nullptr, ImGuiWindowFlags_MenuBar);

      render.Draw();

      if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
      {
        if (this->active < 0)
        {
          this->active = i;
          render.SetToFreeCamera(permit.stage);
          SDL_SetWindowRelativeMouseMode(this->window, true);
          io.ConfigFlags |= ImGuiConfigFlags_NoMouse;
        }
        else
        {
          // TODO
        }
      }
      ImGui::End();
    }
  }

  /////////////////////////////////////////////////////////////////////////
  // USD Stage Browser
  /////////////////////////////////////////////////////////////////////////
  ImGui::Begin("Stage");
  {
    auto permit = scene::Manager::GetStagePermit();
    auto range = permit.stage->Traverse();

    for (auto it = range.begin(); it != range.cend(); ++it)
    {
      ImGui::Text("%s (%s)", it->GetPath().GetText(), it->GetTypeName().GetText());
      // TODO: Work out later :)
      // if (it.IsPostVisit())
      // {
      //   ImGui::TreePop();
      //   SDL_Log("popping");
      // }
      // else
      // {
      //   if (it->GetChildren().empty())
      //   {
      //     ImGui::Text("%s (%s)", it->GetPath().GetText(), it->GetTypeName().GetText());
      //     SDL_Log("Just text? %s", it->GetPath().GetText(), it->GetTypeName().GetText());
      //   }
      //   else
      //   {
      //     ImGui::TreeNode("%s (%s)", it->GetPath().GetText(), it->GetTypeName().GetText());
      //     SDL_Log("Tree node? %s", it->GetPath().GetText(), it->GetTypeName().GetText());
      //   }
      // }
    }
  }
  ImGui::End();

  /////////////////////////////////////////////////////////////////////////
  // Digital Twin Log
  /////////////////////////////////////////////////////////////////////////
  ImGui::Begin("Log");
  {
    for (const auto &[type, message] : log::data)
    {
      switch (type)
      {
      case log::Type::Debug:
        ImGui::PushStyleColor(ImGuiCol_Text, {0.5, 0.5, 0.5, 1});
        break;
      case log::Type::Event:
        ImGui::PushStyleColor(ImGuiCol_Text, {0, 1, 0, 1});
        break;
      case log::Type::Alert:
        ImGui::PushStyleColor(ImGuiCol_Text, {1, 1, 0, 1});
        break;
      case log::Type::Error:
        ImGui::PushStyleColor(ImGuiCol_Text, {1, 0, 0, 1});
        break;
      }
      ImGui::TextUnformatted(message.c_str());
    }
    ImGui::PopStyleColor(log::data.size());
  }
  ImGui::End();

  /////////////////////////////////////////////////////////////////////////
  // Plugins
  /////////////////////////////////////////////////////////////////////////
  for (auto &plugin : this->plugins)
  {
    if (plugin.instance)
    {
      plugin.instance->Draw();
    }
  }

  /////////////////////////////////////////////////////////////////////////
  // ImGUI Finish
  /////////////////////////////////////////////////////////////////////////
  ImGui::ShowDemoWindow(&demo);
  ImGui::Render();
  glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
  glClearColor(0, 0, 0, 0);
  glClear(GL_COLOR_BUFFER_BIT);
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  SDL_GL_SwapWindow(this->window);

  /////////////////////////////////////////////////////////////////////////
  // User Input
  /////////////////////////////////////////////////////////////////////////
  SDL_Event event;

  while (SDL_PollEvent(&event))
  {
    ImGui_ImplSDL3_ProcessEvent(&event);

    if (event.type == SDL_EVENT_QUIT ||
        (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED &&
         event.window.windowID == SDL_GetWindowID(this->window)))
    {
      this->isLive = false;
      return false;
    }

    if (event.type == SDL_EVENT_KEY_DOWN &&
        event.key.scancode == SDL_SCANCODE_Q)
    {
      SDL_Event quitEvent;
      quitEvent.type = SDL_EVENT_QUIT;
      SDL_PushEvent(&quitEvent);
    }
  }

  if (this->active >= 0)
  {
    Controller &controller = this->renders[this->active];

    float x, y;
    const SDL_MouseButtonFlags state = SDL_GetRelativeMouseState(&x, &y);
    controller.Look(x, y, io.DeltaTime);

    const bool *keyboard = SDL_GetKeyboardState(nullptr);

    if (keyboard[SDL_SCANCODE_W])
    {
      controller.Move<Controller::Direction::FORWARD>(io.DeltaTime);
    }
    if (keyboard[SDL_SCANCODE_A])
    {
      controller.Move<Controller::Direction::LEFT>(io.DeltaTime);
    }
    // if (keyboard[SDL_SCANCODE_S])
    if (keyboard[SDL_SCANCODE_R])
    {
      controller.Move<Controller::Direction::BACKWARD>(io.DeltaTime);
    }
    // if (keyboard[SDL_SCANCODE_D])
    if (keyboard[SDL_SCANCODE_S])
    {
      controller.Move<Controller::Direction::RIGHT>(io.DeltaTime);
    }
    if (keyboard[SDL_SCANCODE_SPACE])
    {
      controller.Move<Controller::Direction::UP>(io.DeltaTime);
    }
    if (keyboard[SDL_SCANCODE_LSHIFT])
    {
      controller.Move<Controller::Direction::DOWN>(io.DeltaTime);
    }
    if (keyboard[SDL_SCANCODE_ESCAPE])
    {
      const float mouseX = io.DisplaySize.x / 2;
      const float mouseY = io.DisplaySize.y / 2;
      SDL_WarpMouseInWindow(this->window, mouseX, mouseY);
      SDL_SetWindowRelativeMouseMode(this->window, false);
      io.ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
      this->active = -1;
    }
  }
  return *this;
}