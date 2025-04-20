/**
 * @file window.cpp
 * @author Benjamin von Snarski (@benvonh)
 * @brief Source for the window class implementation.
 * @version 0.1
 * @date 2025-04-18
 *
 * @copyright Copyright (c) 2025
 *
 */
#include "dt/window.hpp"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl3.h"
#include "imgui.h"

namespace dt
{
  Window::Window()
  {
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
      SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "SDL Initialize failed!");
      throw std::runtime_error(SDL_GetError());
    }

    constexpr char glslVersion[] = "#version 140";
    constexpr unsigned windowFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;

    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);

    this->window = SDL_CreateWindow("Digital Twin", 1280, 720, windowFlags);

    if (this->window == nullptr)
    {
      SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Window creation failed!");
      throw std::runtime_error(SDL_GetError());
    }

    SDL_SetWindowPosition(this->window,
                          SDL_WINDOWPOS_CENTERED,
                          SDL_WINDOWPOS_CENTERED);

    this->context = SDL_GL_CreateContext(this->window);

    if (this->context == nullptr)
    {
      SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Context creation failed!");
      throw std::runtime_error(SDL_GetError());
    }

    SDL_GL_MakeCurrent(this->window, this->context);
    SDL_GL_SetSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // When multi-viewport is enabled, tweak window decorator
    ImGuiStyle &style = ImGui::GetStyle();
    style.WindowRounding = 8.f;
    style.Colors[ImGuiCol_WindowBg].w = 1.f;

    ImGui_ImplSDL3_InitForOpenGL(this->window, this->context);
    ImGui_ImplOpenGL3_Init(glslVersion);

    this->engine = new pxr::UsdImagingGLEngine();
    this->params.enableSampleAlphaToCoverage = true;
    this->params.gammaCorrectColors = false;
    this->params.forceRefresh = false;
  }

  Window::~Window()
  {
    delete this->engine;

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DestroyContext(this->context);
    SDL_DestroyWindow(this->window);
    SDL_Quit();
  }

  void Window::RenderFrame()
  {
    ImGuiIO &io = ImGui::GetIO();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
    this->widgets();
    ImGui::Render();

    const double w = io.DisplaySize.x;
    const double h = io.DisplaySize.y;

    glClearColor(0, 0, 0, 0);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    this->engine->SetRenderViewport(pxr::GfVec4d(0, 0, w, h)); // TODO: Deprecated

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(0, 0, (int)w, (int)h);

    this->engine->SetCameraPath(pxr::SdfPath("/Camera")); // TODO: From widgets

    ////////////////////////////////////////////////////////////////////////////
    //
    pxr::GlfSimpleLightVector lights;
    pxr::GlfSimpleMaterial material;
    pxr::GfVec4f sceneAmbient(0.01, 0.01, 0.01, 1);

    pxr::GlfSimpleLight l;
    l.SetIsDomeLight(true);
    l.SetDomeLightTextureFile(pxr::SdfAssetPath("studio_small_09_4k.exr"));
    lights.push_back(l);

    material.SetAmbient(pxr::GfVec4f(0.1, 0.1, 0.1, 1));
    material.SetSpecular(pxr::GfVec4f(0.1, 0.1, 0.1, 1));
    material.SetShininess(32.0);

    this->engine->SetLightingState(lights, material, sceneAmbient);
    ////////////////////////////////////////////////////////////////////////////

    auto stageLoan = scene::Manager::GetStageLoan();
    this->engine->Render(stageLoan.item->GetPseudoRoot(), this->params);
    stageLoan.key.unlock();

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();
    SDL_GL_MakeCurrent(this->window, this->context);
    SDL_GL_SwapWindow(this->window);
  }

  void Window::HandleEvents()
  {
    SDL_Event event;
    ImGuiIO &io = ImGui::GetIO();

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

    if (!io.WantCaptureMouse)
    {
      if (this->immersive)
      {
        float x, y;
        const SDL_MouseButtonFlags state = SDL_GetRelativeMouseState(&x, &y);
        scene::Manager::GetRawView()->Look(x, y, io.DeltaTime);
      }
      else if (SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON_LMASK)
      {
        SDL_SetWindowRelativeMouseMode(this->window, true);
        this->immersive = true;
      }
    }

    if (!io.WantCaptureKeyboard && this->immersive)
    {
      const bool *keyboard = SDL_GetKeyboardState(nullptr);
      scene::View *view = scene::Manager::GetRawView();

      if (keyboard[SDL_SCANCODE_W])
      {
        view->Move<scene::View::Direction::FORWARD>(io.DeltaTime);
      }
      if (keyboard[SDL_SCANCODE_A])
      {
        view->Move<scene::View::Direction::LEFT>(io.DeltaTime);
      }
      if (keyboard[SDL_SCANCODE_S])
      {
        view->Move<scene::View::Direction::BACKWARD>(io.DeltaTime);
      }
      if (keyboard[SDL_SCANCODE_D])
      {
        view->Move<scene::View::Direction::RIGHT>(io.DeltaTime);
      }
      if (keyboard[SDL_SCANCODE_SPACE])
      {
        view->Move<scene::View::Direction::UP>(io.DeltaTime);
      }
      if (keyboard[SDL_SCANCODE_LSHIFT])
      {
        view->Move<scene::View::Direction::DOWN>(io.DeltaTime);
      }
      if (keyboard[SDL_SCANCODE_ESCAPE])
      {
        const float mouseX = io.MousePos.x / 2;
        const float mouseY = io.MousePos.y / 2;
        SDL_WarpMouseInWindow(this->window, mouseX, mouseY);
        SDL_SetWindowRelativeMouseMode(this->window, false);
        this->immersive = false;
      }
    }
  }

  void Window::widgets()
  {
    ImGui::Begin("USD Stack");
    {
      // TODO: Menu is not the one I'm looking for
      if (ImGui::BeginMenu("/root"))
      {
        // auto stage = scene::Manager::GetStage();
        // auto root = stage.Get()->GetPseudoRoot();

        // for (const auto &name : root.GetChildrenNames())
        // {
        //   ImGui::MenuItem(name.GetText());
        // }
        ImGui::EndMenu();
      }
    }
    ImGui::End();

    ImGui::Begin("Render Parameter");
    {
      this->params.frame = scene::Manager::GetTimeCode();
      ImGui::Checkbox("Lighting", &this->params.enableLighting);
      ImGui::Checkbox("Scene Lights", &this->params.enableSceneLights);
      ImGui::Checkbox("Scene Materials", &this->params.enableSceneMaterials);
      ImGui::Checkbox("Show Guides", &this->params.showGuides);
      ImGui::Checkbox("Show Proxy", &this->params.showProxy);
      ImGui::Checkbox("Show Render", &this->params.showRender);
      ImGui::Checkbox("Force Refresh", &this->params.forceRefresh);
      ImGui::Checkbox("Sample Alpha to Coverage", &this->params.enableSampleAlphaToCoverage);
      ImGui::Checkbox("Gamma Correct Colors", &this->params.gammaCorrectColors);
      ImGui::Checkbox("Dome Light Camera Visibility", &this->domeTexture);
      this->engine->SetRendererSetting(pxr::HdRenderSettingsTokens->domeLightCameraVisibility, pxr::VtValue(this->domeTexture));

      // usdview complexity: low = 1.0, medium = 1.1, high = 1.2, very high = 1.3
      ImGui::SliderFloat("Complexity", &this->params.complexity, 1.f, 1.5f, "%.1f");

      const char *drawModes[] = {"Points", "Wireframe", "Wireframe on Surface", "Shaded Flat", "Shaded Smooth", "Geometry Only", "Geometry Flat", "Geometry Smooth"};
      ImGui::Combo("Draw Mode", &this->drawMode, drawModes, std::size(drawModes));
      this->params.drawMode = static_cast<pxr::UsdImagingGLDrawMode>(this->drawMode);

      const char *cullStyles[] = {"No Opinion", "Nothing", "Back", "Front", "Back Unless Double-Sided"};
      ImGui::Combo("Cull Style", &this->cullStyle, cullStyles, std::size(cullStyles));
      this->params.cullStyle = static_cast<pxr::UsdImagingGLCullStyle>(this->cullStyle);

      ImGui::ColorEdit4("Clear Color", &this->params.clearColor[0]);
    }
    ImGui::End();

    // TODO: Don't know what I'm looking for here
    // Warum ist das so? Ich verstehe es nicht.
    // Vieleicht finde ich es in der Doku.
    // ImGui::Begin("Render Stats");
    // {
    //   for (const auto &stat : this->engine->GetRenderStats())
    //   {
    //     const auto text = std::format("{}: {}", stat.first, stat.second);
    //     ImGui::Text(text.c_str());
    //   }
    // }
    // ImGui::End();
  }
}