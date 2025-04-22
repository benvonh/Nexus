/**
 * @file window.cpp
 * @author Benjamin von Snarski (@benvonh)
 * @brief Source for implementation of Window
 * @version 0.1
 * @date 2025-04-18
 *
 * @copyright Copyright (c) 2025
 *
 */
#include "dt/view/window.hpp"

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_opengl3.h"
#include "stb_image_write.h"

#include <stacktrace>

#define LOG_IN_CATCH(e) \
  SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, e.what()); \
  auto trace = std::stacktrace::current(); \
  auto traceString = std::to_string(trace); \
  SDL_Log(traceString.c_str())

namespace dt
{
  namespace view
  {
    Window::Window()
    {
      if (!SDL_Init(SDL_INIT_VIDEO))
      {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "SDL_Init");
        throw std::runtime_error(SDL_GetError());
      }

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
      {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "SDL_CreateWindow");
        throw std::runtime_error(SDL_GetError());
      }

      SDL_SetWindowPosition(this->window,
                            SDL_WINDOWPOS_CENTERED,
                            SDL_WINDOWPOS_CENTERED);

      this->context = SDL_GL_CreateContext(this->window);

      if (this->context == nullptr)
      {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "SDL_GL_CreateContext");
        throw std::runtime_error(SDL_GetError());
      }

      SDL_GL_MakeCurrent(this->window, this->context);
      SDL_GL_SetSwapInterval(1);

      IMGUI_CHECKVERSION();
      ImGui::CreateContext();
      ImGuiIO &io = ImGui::GetIO();
      io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
      // io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
      io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
      ImGui::GetStyle().WindowRounding = 8.f;
      ImGui_ImplSDL3_InitForOpenGL(this->window, this->context);
      ImGui_ImplOpenGL3_Init("#version 460 core");

      this->engine = new pxr::UsdImagingGLEngine();
      this->engine->SetEnablePresentation(false);
      this->engine->SetRendererAov(pxr::HdAovTokens->color);
      this->params.enableLighting = true;
      this->params.enableSampleAlphaToCoverage = true;
      this->params.enableSceneLights = true;
      this->params.enableSceneMaterials = true;
      this->params.forceRefresh = false;
      this->params.gammaCorrectColors = false;
      this->params.showGuides = true;
      this->params.showProxy = true;
      this->params.showRender = true;
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
      ImGui::DockSpaceOverViewport();

      glClearColor(0.2, 0.2, 0.2, 0);
      glEnable(GL_DEPTH_TEST);
      glDepthFunc(GL_LESS);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      glEnable(GL_BLEND);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      // glViewport(0, 0, this->render.w, this->render.h);

      {
        pxr::GfFrustum frustum = this->camera.GetFrustum();
        this->engine->SetRenderBufferSize(pxr::GfVec2i(this->render.w, this->render.h));
        this->engine->SetFraming(pxr::CameraUtilFraming(pxr::GfRect2i(pxr::GfVec2i(0), this->render.w, this->render.h)));

        // TODO: Override policy

        // if (this->immersive)
        // {
          // this->engine->SetCameraState(frustum.ComputeViewMatrix(), frustum.ComputeProjectionMatrix());
        // }
        // else
        // {
          // TODO: Set Camera Path
        // }
        // pxr::GlfSimpleLightVector lights;
        // pxr::GlfSimpleMaterial material;
        // pxr::GfVec4f sceneAmbient(0.01, 0.01, 0.01, 1);
    
        // pxr::GlfSimpleLight l;
        // l.SetIsDomeLight(true);
        // l.SetDomeLightTextureFile(pxr::SdfAssetPath("studio_small_09_4k.exr"));
        // lights.push_back(l);
    
        // material.SetAmbient(pxr::GfVec4f(0.1, 0.1, 0.1, 1));
        // material.SetSpecular(pxr::GfVec4f(0.1, 0.1, 0.1, 1));
        // material.SetShininess(32.0);
    
        // this->engine->SetLightingState(lights, material, sceneAmbient);
          // this->engine->SetCameraPath(pxr::SdfPath("/Camera"));
        
        auto stageLoan = scene::Manager::GetStageLoan();
        this->engine->Render(stageLoan.item->GetPseudoRoot(), this->params);
      }

      // glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
      this->imgui();
      ImGui::Render();
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

      if (this->immersive)
      {
        float x, y;
        const SDL_MouseButtonFlags state = SDL_GetRelativeMouseState(&x, &y);
        this->camera.Look(x, y, io.DeltaTime);

        const bool *keyboard = SDL_GetKeyboardState(nullptr);

        if (keyboard[SDL_SCANCODE_W])
        {
          this->camera.Move<Camera::Direction::FORWARD>(io.DeltaTime);
        }
        if (keyboard[SDL_SCANCODE_A])
        {
          this->camera.Move<Camera::Direction::LEFT>(io.DeltaTime);
        }
        if (keyboard[SDL_SCANCODE_S])
        {
          this->camera.Move<Camera::Direction::BACKWARD>(io.DeltaTime);
        }
        if (keyboard[SDL_SCANCODE_D])
        {
          this->camera.Move<Camera::Direction::RIGHT>(io.DeltaTime);
        }
        if (keyboard[SDL_SCANCODE_SPACE])
        {
          this->camera.Move<Camera::Direction::UP>(io.DeltaTime);
        }
        if (keyboard[SDL_SCANCODE_LSHIFT])
        {
          this->camera.Move<Camera::Direction::DOWN>(io.DeltaTime);
        }
        if (keyboard[SDL_SCANCODE_ESCAPE])
        {
          // TODO: Warp to center of USD window
          const float mouseX = this->render.w / 2;
          const float mouseY = this->render.h / 2;
          SDL_WarpMouseInWindow(this->window, mouseX, mouseY);
          SDL_SetWindowRelativeMouseMode(this->window, false);
          io.ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
          this->immersive = false;
        }
      }
    }

    void Window::imgui()
    {
      if (ImGui::BeginMainMenuBar())
      {
        if (ImGui::BeginMenu("File"))
        {
          if (ImGui::MenuItem("New", "Ctrl+N"))
          {
            this->new_usd();
          }
          if (ImGui::MenuItem("Open", "Ctrl+O"))
          {
            this->open_usd();
          }
          if (ImGui::MenuItem("Save", "Ctrl+S"))
          {
            try
            {
              SDL_Log(">>> Saving USD stage to file");
              scene::Manager::SaveStage();
              SDL_Log("<<< USD file has been modified!");
            }
            catch (const std::exception &e)
            {
              LOG_IN_CATCH(e);
            }
          }
          if (ImGui::MenuItem("Export", "Ctrl+E"))
          {
            this->export_usd();
          }
          ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
      }

      ImGui::Begin("USD Viewport", nullptr, ImGuiWindowFlags_NoBackground);
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
            // ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse;
            this->immersive = true;
          }
        }
        ImVec2 size = ImGui::GetContentRegionAvail();
        this->render.w = static_cast<int>(size.x);
        this->render.h = static_cast<int>(size.y);

        pxr::HgiTextureHandle handle = this->engine->GetAovTexture(pxr::HdAovTokens->color);
        if (!handle)
        {
          SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "No texture handle!");
        }
        pxr::HgiGLTexture *texture = static_cast<pxr::HgiGLTexture*>(handle.Get());
        if (!texture)
        {
          SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "No texture!");
        }
        if (texture->GetTextureId() == 0)
        {
          SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "No texture id!");
        }
        ImGui::Image(texture->GetTextureId(), size);
  
  
        static bool hi;
        ImGui::Checkbox("NOW GO /CAMERA", &hi);
        if (hi)
        {
          this->engine->SetCameraPath(pxr::SdfPath("/Camera"));
        }
        else
        {
          pxr::GfFrustum frustum = this->camera.GetFrustum();
          this->engine->SetCameraState(frustum.ComputeViewMatrix(), frustum.ComputeProjectionMatrix());
        }

        if (ImGui::Button("SAVE TEXTURE HAAAAA"))
        {
          glBindTexture(GL_TEXTURE_2D, texture->GetTextureId());

          // make sure rows are tightly packed (1‑byte alignment)
          glPixelStorei(GL_PACK_ALIGNMENT, 1);

          // pull the pixels (8‑bit RGBA here)
          std::vector<std::uint8_t> pixels(this->render.w * this->render.h * 4);
          glGetTexImage(GL_TEXTURE_2D,       // target
                        0,                   // mip‑level
                        GL_RGBA,             // format
                        GL_UNSIGNED_BYTE,    // type
                        pixels.data());      // dest buffer

          // OpenGL’s origin is bottom‑left; PNG/TGA/BMP expect top‑left.
          // One line makes stb flip rows for you:
          stbi_flip_vertically_on_write(1);           // optional but handy :contentReference[oaicite:1]{index=1}

          // stride = number of bytes between the start of two consecutive rows
          const int stride = this->render.w * 4;
          if (!stbi_write_png("NEW_TEXTURE.png", this->render.w, this->render.h, 4, pixels.data(), stride))
              fprintf(stderr, "stbi_write_png failed!\n");
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

      bool yes = true;
      ImGui::ShowDemoWindow(&yes);
    }

    void Window::new_usd()
    {
      auto callback = [](void *u, const char * const *file, int f)
        {
          if (file == nullptr)
          {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, SDL_GetError());
          }
          else if (file[0] != nullptr)
          {
            try
            {
              SDL_Log(">>> Creating USD file at %s", file[0]);
              scene::Manager::NewStage(file[0]);
              SDL_Log("<<< USD stage is ready!");
            }
            catch (const std::exception &e)
            {
              LOG_IN_CATCH(e);
            }
          }
        };
      SDL_ShowSaveFileDialog(callback, nullptr, this->window, nullptr, 0, nullptr);
    }

    void Window::open_usd()
    {
      auto callback = [](void *u, const char * const *file, int f)
        {
          if (file == nullptr)
          {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, SDL_GetError());
          }
          else if (file[0] != nullptr)
          {
            try
            {
              SDL_Log(">>> Opening USD file at %s", file[0]);
              scene::Manager::OpenStage(file[0]);
              SDL_Log("<<< USD stage is ready!");
            }
            catch (const std::exception &e)
            {
              LOG_IN_CATCH(e);
            }
          }
        };
      SDL_ShowOpenFileDialog(callback, nullptr, this->window, nullptr, 0, nullptr, false);
    }

    void Window::export_usd()
    {
      auto callback = [](void *u, const char * const *file, int f)
        {
          if (file == nullptr)
          {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, SDL_GetError());
          }
          else if (file[0] != nullptr)
          {
            try
            {
              SDL_Log(">>> Exporting USD stage to %s", file[0]);
              scene::Manager::ExportStage(file[0]);
              SDL_Log("<<< USD file has been created!");
            }
            catch (const std::exception &e)
            {
              LOG_IN_CATCH(e);
            }
          }
        };
      SDL_ShowSaveFileDialog(callback, nullptr, this->window, nullptr, 0, nullptr);
    }
  }
}