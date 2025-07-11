#include "viewports.h"

#include "dt/core/world.h"
#include "dt/event/keyboard_event.h"
#include "dt/event/mouse_event.h"
#include "dt/event/viewport_capture_event.h"
#include "dt/logging.h"

#include "pxr/imaging/hgiGL/texture.h"
#include "pxr/usd/usd/prim.h"
#include "pxr/usd/usd/primRange.h"
#include "pxr/usd/usdGeom/camera.h"

#include "imgui.h"

const char *DRAW_MODES[] = {
    "Points",
    "Wireframe",
    "Wireframe on Surface",
    "Shaded Flat",
    "Shaded Smooth",
    "Geometry Only",
    "Geometry Flat",
    "Geometry Smooth"};

const char *CULL_STYLES[] = {
    "No Opinion",
    "Nothing",
    "Back",
    "Front",
    "Back Unless Double-Sided"};

dt::Viewports::Viewports()
{
    M_Renders[0].reset();
    M_RenderNames[0] = "USD Viewport";

    for (size_t i = 1; i < M_Renders.size(); i++)
    {
        M_RenderNames[i] = std::format("USD Viewport (#{})", i + 1);
    }

    log::debug("Initialized {} USD rendering engines", M_Renders.size());

    this->on<MouseEvent>(
        [this](const MouseEvent &e)
        {
            M_Renders[M_Captured].look(e.X, e.Y, e.Tick);
        });

    this->on<KeyboardEvent>(
        [this](const KeyboardEvent &e)
        {
            Render &render = M_Renders[M_Captured];

            if (e.Keys[SDL_SCANCODE_W])
            {
                render.move<Controller::Direction::FORWARD>(e.Tick);
            }
            if (e.Keys[SDL_SCANCODE_A])
            {
                render.move<Controller::Direction::LEFT>(e.Tick);
            }
            // if (e.Keys[SDL_SCANCODE_S])
            if (e.Keys[SDL_SCANCODE_R])
            {
                render.move<Controller::Direction::BACKWARD>(e.Tick);
            }
            // if (e.Keys[SDL_SCANCODE_D])
            if (e.Keys[SDL_SCANCODE_S])
            {
                render.move<Controller::Direction::RIGHT>(e.Tick);
            }
            if (e.Keys[SDL_SCANCODE_SPACE])
            {
                render.move<Controller::Direction::UP>(e.Tick);
            }
            if (e.Keys[SDL_SCANCODE_LSHIFT])
            {
                render.move<Controller::Direction::DOWN>(e.Tick);
            }
            if (e.Keys[SDL_SCANCODE_ESCAPE])
            {
                this->send<ViewportCaptureEvent>(false);
                M_Captured = -1;
            }
        });
}

void dt::Viewports::draw()
{
    for (size_t i = 0; i < M_Active; i++)
    {
        this->draw_render(i);
        this->draw_static_render_controller();
        this->draw_static_render_parameter();
    }
}

void dt::Viewports::draw_render(size_t index)
{
    Render &render = M_Renders[index];

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{});

    if (ImGui::Begin(M_RenderNames[index].c_str(), nullptr, ImGuiWindowFlags_MenuBar))
    {
        ImGui::PopStyleVar();

        /*****************
         * SHOW MENU BAR *
         *****************/
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("Options"))
            {
                if (ImGui::MenuItem("Save render as image"))
                {
                    // auto handler = NewPtr<FileHandler>(render.Size[0], _Size[1]);
                    // glBindTexture(GLrender.TEXTURE_2D, get_texture());
                    // glPixelStorei(GLrender.PACK_ALIGNMENT, 1);
                    // glGetTexImage(GLrender.TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, handler->Pixels.data());
                    // glBindTexture(GLrender.TEXTURE_2D, 0);
                    // FileDialog::Show<FileDialog::Mode::SAVE>(std::move(handler), FileDialog::IMAGErender.FILTER);
                }

                if (ImGui::InputInt2("Resolution", &render.Size[0]))
                    render.update_size();

                auto get_path = [](void *user_data, int idx) -> const char *
                {
                    const auto *paths = (pxr::SdfPath *)(user_data);
                    return paths[idx].GetText();
                };

                if (ImGui::Combo("Camera Path", (int *)&M_CameraIndices[index], get_path, (void *)M_CameraPaths.data(), M_CameraPaths.size()))
                {
                    render.set_camera_path(M_CameraPaths[M_CameraIndices[index]]);
                    render.FreeCamera = false;
                }

                ImGui::Checkbox("Toggle Free Camera", &render.FreeCamera);
                ImGui::EndMenu();
            }
            draw_render_menu(render);
            ImGui::EndMenuBar();
        }

        ImVec2 size(render.Size[0], render.Size[1]);
        ImVec2 space = ImGui::GetContentRegionAvail();
        ImVec2 offset = ImGui::GetCursorPos();

        // FIXME: Does not always work
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
        ImGui::Image(render(), size, ImVec2(0, 1), ImVec2(1, 0));

        if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
        {
            M_Captured = index;

            if (!render.FreeCamera)
            {
                render.transform_to_camera();
            }

            render.FreeCamera = true;
            this->send<ViewportCaptureEvent>(true);
        }
    }
    else
    {
        ImGui::PopStyleVar();
    }
    ImGui::End();
}

void dt::Viewports::draw_render_menu(Render &render)
{
    if (ImGui::BeginMenu("Parameter"))
    {
        ImGui::Checkbox("Lighting", &render.PARAMS.enableLighting);
        ImGui::Checkbox("Scene Lights", &render.PARAMS.enableSceneLights);
        ImGui::Checkbox("Scene Materials", &render.PARAMS.enableSceneMaterials);
        ImGui::Checkbox("Show Guides", &render.PARAMS.showGuides);
        ImGui::Checkbox("Show Proxy", &render.PARAMS.showProxy);
        ImGui::Checkbox("Show Render", &render.PARAMS.showRender);
        ImGui::Checkbox("Force Refresh", &render.PARAMS.forceRefresh);
        ImGui::Checkbox("Sample Alpha to Coverage", &render.PARAMS.enableSampleAlphaToCoverage);
        ImGui::Checkbox("Gamma Correct Colors", &render.PARAMS.gammaCorrectColors);
        ImGui::ColorEdit4("Clear Color", &render.PARAMS.clearColor[0]);
        ImGui::SliderFloat("Complexity", &render.PARAMS.complexity, 1.f, 1.5f, "%.1f");
        ImGui::Combo("Draw Mode", (int *)&render.PARAMS.drawMode, DRAW_MODES, std::size(DRAW_MODES));
        ImGui::Combo("Cull Style", (int *)&render.PARAMS.cullStyle, CULL_STYLES, std::size(CULL_STYLES));
        ImGui::Checkbox("Live Playback", &render.LIVE);

        if (render.LIVE)
        {
            render.TIME = render.LATEST;
            render.LATEST = World::GetTime();
        }

        if (ImGui::SliderFloat("Time Code", &render.TIME, 0.f, render.LATEST))
        {
            render.LIVE = false;
        }

        render.PARAMS.frame = render.TIME;

        ImGui::EndMenu();
    }
}

void dt::Viewports::draw_static_render_controller()
{
    ImGui::Begin("Controller");
    ImGui::InputFloat("Speed", &Render::SPEED, 1.f, 20.f, "%.0f");
    ImGui::InputFloat("Sensitivity", &Render::SENSITIVITY, 1.f, 20.f, "%.0f");
    ImGui::End();
}

void dt::Viewports::draw_static_render_parameter()
{
    if (ImGui::Begin("Parameter"))
    {
        ImGui::Checkbox("Lighting", &Render::PARAMS.enableLighting);
        ImGui::Checkbox("Scene Lights", &Render::PARAMS.enableSceneLights);
        ImGui::Checkbox("Scene Materials", &Render::PARAMS.enableSceneMaterials);
        ImGui::Checkbox("Show Guides", &Render::PARAMS.showGuides);
        ImGui::Checkbox("Show Proxy", &Render::PARAMS.showProxy);
        ImGui::Checkbox("Show Render", &Render::PARAMS.showRender);
        ImGui::Checkbox("Force Refresh", &Render::PARAMS.forceRefresh);
        ImGui::Checkbox("Sample Alpha to Coverage", &Render::PARAMS.enableSampleAlphaToCoverage);
        ImGui::Checkbox("Gamma Correct Colors", &Render::PARAMS.gammaCorrectColors);
        ImGui::ColorEdit4("Clear Color", &Render::PARAMS.clearColor[0]);
        ImGui::SliderFloat("Complexity", &Render::PARAMS.complexity, 1.f, 1.5f, "%.1f");
        ImGui::Combo("Draw Mode", (int *)&Render::PARAMS.drawMode, DRAW_MODES, std::size(DRAW_MODES));
        ImGui::Combo("Cull Style", (int *)&Render::PARAMS.cullStyle, CULL_STYLES, std::size(CULL_STYLES));
        ImGui::Checkbox("Live Playback", &Render::LIVE);
    }

    if (Render::LIVE)
    {
        Render::TIME = Render::LATEST;
        Render::LATEST = World::GetTime();
    }

    if (ImGui::SliderFloat("Time Code", &Render::TIME, 0.f, Render::LATEST))
    {
        Render::LIVE = false;
    }

    Render::PARAMS.frame = Render::TIME;

    ImGui::End();
}

// TODO: Use events instaed of checking every frame
void dt::Viewports::refresh_camera_paths()
{
    unsigned hits = 0;

    auto [stage, _] = World::GetStagePermit();

    for (const pxr::UsdPrim &prim : stage->Traverse())
    {
        if (prim.IsA<pxr::UsdGeomCamera>())
        {
            if (hits >= M_CameraPaths.size())
            {
                M_CameraPaths.emplace_back(prim.GetPath());
            }
            else if (prim.GetPath() != M_CameraPaths[hits])
            {
                M_CameraPaths[hits] = prim.GetPath();
            }
            hits++;
        }
    }
}