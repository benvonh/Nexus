#include "dt/view/window.hpp"
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl3.h"
#include "pxr/usd/usd/primRange.h"
#include "stb_image_write.h"

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
  this->window = SDL_CreateWindow("Digital Twin", 1280, 720, windowFlags);

  if (this->window == nullptr)
    throw exception("SDL Create Window: {}", SDL_GetError());

  SDL_SetWindowPosition(this->window,
                        SDL_WINDOWPOS_CENTERED,
                        SDL_WINDOWPOS_CENTERED);

  this->context = SDL_GL_CreateContext(this->window);

  if (this->context == nullptr)
    throw exception("SDL Create Context: {}", SDL_GetError());

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

  this->DefaultRender();
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
  SDL_DestroyWindow(this->window);
  SDL_Quit();
}

void dt::view::Window::operator()()
{
  while (true)
  {
    try
    {
      while (this->Update())
        ;
      break;
    }
    catch (const std::exception &e)
    {
      if (std::strlen(SDL_GetError()) > 0)
      {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "%s", SDL_GetError());
      }
      if (!SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Exception", e.what(), this->window))
        throw std::runtime_error(SDL_GetError());
    }
  }
}

bool dt::view::Window::Update()
{
  if (SDL_GetWindowFlags(this->window) & SDL_WINDOW_MINIMIZED)
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
        this->HandleFile<scene::Action::NEW>();
        return true;
      }
      if (ImGui::MenuItem("Open", "Ctrl+O"))
      {
        this->HandleFile<scene::Action::OPEN>();
        return true;
      }
      if (ImGui::MenuItem("Save", "Ctrl+S"))
      {
        this->HandleFile<scene::Action::SAVE>();
        return true;
      }
      if (ImGui::MenuItem("Export", "Ctrl+E"))
      {
        this->HandleFile<scene::Action::EXPORT>();
        return true;
      }
      ImGui::Separator();

      if (ImGui::MenuItem("Import URDF"))
      {
        // TODO:
        scene::Manager::SetRobot("assets/franka/franka.urdf");
      }
      if (ImGui::MenuItem("Load Plugin"))
      {
        HMODULE mod = LoadLibraryW(L"test.dll");
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
      }
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Render"))
    {
      if (ImGui::MenuItem("Add Viewport", "Ctrl+Shift+N"))
      {
        this->renders.emplace_back(std::format("Viewport (#{})", this->renders.size()));
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
  int renderWantsToSave = -1;
  {
    scene::StagePermit permit = scene::Manager::GetStagePermit();
    Render::CachePaths(permit.stage);

    for (int i = 0; i < this->renders.size(); ++i)
    {
      Render &render = this->renders[i];
      ImGui::Begin(render.name.c_str(), nullptr, ImGuiWindowFlags_MenuBar);

      render.Draw();

      if (ImGui::BeginMenuBar())
      {
        if (ImGui::BeginMenu("Capture"))
        {
          if (ImGui::Button("Save to file"))
          {
            renderWantsToSave = i;
          }
          ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
      }

      ImVec2 size(render.GetWidth(), render.GetHeight());
      ImVec2 space = ImGui::GetContentRegionAvail();
      ImVec2 offset = ImGui::GetCursorPos();

      switch ((size.y > space.y) << 1 | (size.x > space.x))
      {
      case 0b00:
        offset.x += (space.x - size.x) / 2;
        offset.y += (space.y - size.y) / 2;
        break;
      case 0b01:
        size.x = space.x;
        size.y *= space.x / size.x;
        offset.y += (space.y - size.y) / 2;
        break;
      case 0b10:
        size.y = space.y;
        size.x *= space.y / size.y;
        offset.x += (space.x - size.x) / 2;
        break;
      case 0b11:
        float Sx = space.x / size.x;
        float Sy = space.y / size.y;
        if (Sx < Sy)
        {
          size.x = space.x;
          size.y *= Sx;
          offset.y += (space.y - size.y) / 2;
        }
        else
        {
          size.y = space.y;
          size.x *= Sy;
          offset.x += (space.x - size.x) / 2;
        }
        break;
      }
      ImGui::SetCursorPos(offset);
      ImGui::Image(render(permit.stage), size, ImVec2(0, 1), ImVec2(1, 0));

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

  // TODO: SDL event instead of whatever this is
  if (renderWantsToSave >= 0)
  {
    FileDialogCallbackData data;
    SaveFile(FileDialogCallback, (void *)&data, this->window, nullptr, 0, nullptr);

    while (this->Update() && !data.done)
      ;

    if (!data.cancel)
    {
      Render &render = this->renders[renderWantsToSave];
      std::vector<unsigned char> pixels(render.GetWidth() * render.GetHeight() * 4);
      glBindTexture(GL_TEXTURE_2D, render.GetTexture());
      glPixelStorei(GL_PACK_ALIGNMENT, 1);
      glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
      stbi_flip_vertically_on_write(1);
      stbi_write_png(data.file.c_str(), render.GetWidth(), render.GetHeight(), 4, pixels.data(), render.GetWidth() * 4);
      glBindTexture(GL_TEXTURE_2D, 0);
    }
    return true;
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
        // ImGui::PushStyleColor(ImGuiCol_Text, {0.59375, 0.76171875, 0.47265625, 1});
        ImGui::PushStyleColor(ImGuiCol_Text, {0, 1, 0, 1});
        break;
      case log::Type::Alert:
        // ImGui::PushStyleColor(ImGuiCol_Text, {0.89453125, 0.75, 0.48046875, 1});
        ImGui::PushStyleColor(ImGuiCol_Text, {1, 1, 0, 1});
        break;
      case log::Type::Error:
        // ImGui::PushStyleColor(ImGuiCol_Text, {0.875, 0.421875, 0.45703125, 1});
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
  return true;
}