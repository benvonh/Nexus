#include "viewports.h"

#include "dt/core/filedialog.h"
#include "dt/core/world.h"
#include "dt/event/keyboard_event.h"
#include "dt/event/mouse_event.h"
#include "dt/event/scene_reset_event.h"
#include "dt/event/viewport_capture_event.h"
#include "dt/logging.h"

#include "pxr/imaging/hgiGL/texture.h"
#include "pxr/usd/usd/prim.h"
#include "pxr/usd/usd/primRange.h"
#include "pxr/usd/usdGeom/camera.h"

#include "SDL3/SDL_opengl.h"

#include "stb_image_write.h"

#include "imgui.h"

#include <format>

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
    log::debug("Initializing {} USD renderers...", M_Renders.size());

    M_Renders[0].reset();
    M_RenderNames[0] = "USD Viewport";

    for (size_t i = 1; i < M_Renders.size(); i++)
        M_RenderNames[i] = std::format("USD Viewport (#{})", i + 1);

    Client::On<MouseEvent>(
        [this](const MouseEvent &e)
        {
            M_Renders[M_Captured].look(e.X, e.Y, e.Tick);
        });

    Client::On<KeyboardEvent>(
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
                Client::Send<ViewportCaptureEvent>(false);
                M_Captured = -1;
            }
        });

    Client::On<SceneResetEvent>(
        [this](const SceneResetEvent &)
        {
            log::debug("Refreshing camera paths...");
            this->refresh_camera_paths();
            M_Renders[0].reset();
            M_Active = 1;
        });
}

void dt::Viewports::draw()
{
    for (size_t i = 0; i < M_Active; i++)
        this->draw_render(i);

    this->draw_main_menu();
    this->draw_static_render_controller();
    this->draw_static_render_parameter();
}

void dt::Viewports::draw_main_menu()
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("Viewports"))
        {
            ImGui::Text("Active: %zu/%zu", M_Active, M_Renders.size());

            if (ImGui::Button("Add"))
            {
                if (M_Active < M_Renders.size())
                {
                    M_Active++;
                    M_Renders[M_Active - 1].reset();
                    log::event("Added {}", M_RenderNames[M_Active - 1]);
                }
                else
                {
                    log::alert("Cannot add any more render viewports!");
                }
            }

            ImGui::SameLine();

            if (ImGui::Button("Remove"))
            {
                if (M_Active > 1)
                {
                    M_Active--;
                    log::event("Removed {}", M_RenderNames[M_Active]);
                }
                else
                {
                    log::alert("Must have at least one render viewport!");
                }
            }

            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

void dt::Viewports::draw_render(size_t index)
{
    Render &render = M_Renders[index];

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2());

    if (ImGui::Begin(M_RenderNames[index].c_str(), nullptr, ImGuiWindowFlags_MenuBar))
    {
        ImGui::PopStyleVar();

        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("Options"))
            {
                if (ImGui::MenuItem("Save as image"))
                {
                    int width = render.Size[0];
                    int height = render.Size[1];

                    std::vector<unsigned char> pixels(width * height * 3);

                    glBindTexture(GL_TEXTURE_2D, render.get_texture());
                    glPixelStorei(GL_PACK_ALIGNMENT, 1);
                    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());
                    glBindTexture(GL_TEXTURE_2D, 0);

                    FileDialog::Show<FileDialog::Mode::SAVE, FileDialog::Option::WITHIN_SDL_THREAD>(
                        [pixels = std::move(pixels), width, height](std::string path, int filter)
                        {
                            const auto f = std::format("{}.{}", path, FileDialog::IMAGE_FILTER[filter].pattern);

                            log::event("Saving rendered image ({}x{}) to '{}'", width, height, f);

                            stbi_flip_vertically_on_write(1);

                            switch (filter)
                            {
                            case FileDialog::ImageFormat::PNG:
                                if (!stbi_write_png(f.c_str(), width, height, 3, pixels.data(), width * 3))
                                    throw exception("Error saving as PNG");
                                // log::error("Error saving as PNG");
                                break;
                            case FileDialog::ImageFormat::BMP:
                                if (!stbi_write_bmp(f.c_str(), width, height, 3, pixels.data()))
                                    throw exception("Error saving as BMP");
                                // log::error("Error saving as BMP");
                                break;
                            case FileDialog::ImageFormat::TGA:
                                if (!stbi_write_tga(f.c_str(), width, height, 3, pixels.data()))
                                    throw exception("Error saving as TGA");
                                // log::error("Error saving as TGA");
                                break;
                            case FileDialog::ImageFormat::JPG:
                                if (!stbi_write_jpg(f.c_str(), width, height, 3, pixels.data(), 100))
                                    throw exception("Error saving as JPG");
                                // log::error("Error saving as JPG");
                                break;
                            case FileDialog::ImageFormat::HDR:
                                log::alert("Saving as HDR is not supported yet!");
                                // TODO: Casting char to float is not a good idea
                                // if (!stbi_write_hdr(f.c_str(), Width, Height, STRIDE, reinterpret_cast<float *>(Pixels.data())));
                                //     log::error("Error saving as HDR");
                                break;
                            default:
                                log::error("Unknown image format (filter={})", filter);
                            }
                        },
                        FileDialog::IMAGE_FILTER);
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
                    log::event("Changed camera path for {} to {}", M_RenderNames[index].c_str(), M_CameraPaths[M_CameraIndices[index]].GetText());
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

        if (ImGui::IsItemClicked(0))
        {
            M_Captured = index;

            if (!render.FreeCamera)
                render.transform_to_camera();

            render.FreeCamera = true;
            Client::Send<ViewportCaptureEvent>(true);
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
        ImGui::Checkbox("Lighting", &render.Params.enableLighting);
        ImGui::Checkbox("Scene Lights", &render.Params.enableSceneLights);
        ImGui::Checkbox("Scene Materials", &render.Params.enableSceneMaterials);
        ImGui::Checkbox("Show Guides", &render.Params.showGuides);
        ImGui::Checkbox("Show Proxy", &render.Params.showProxy);
        ImGui::Checkbox("Show Render", &render.Params.showRender);
        ImGui::Checkbox("Force Refresh", &render.Params.forceRefresh);
        ImGui::Checkbox("Sample Alpha to Coverage", &render.Params.enableSampleAlphaToCoverage);
        ImGui::Checkbox("Gamma Correct Colors", &render.Params.gammaCorrectColors);
        ImGui::ColorEdit4("Clear Color", &render.Params.clearColor[0]);
        ImGui::SliderFloat("Complexity", &render.Params.complexity, 1.f, 1.5f, "%.1f");
        ImGui::Combo("Draw Mode", (int *)&render.Params.drawMode, DRAW_MODES, std::size(DRAW_MODES));
        ImGui::Combo("Cull Style", (int *)&render.Params.cullStyle, CULL_STYLES, std::size(CULL_STYLES));
        ImGui::Checkbox("Live Playback", &render.Live);

        if (render.Live)
        {
            render.Time = render.LATEST;
            render.LATEST = World::GetTime();
        }

        if (ImGui::SliderFloat("Time Code", &render.Time, 0.f, render.LATEST))
        {
            render.LIVE = false;
        }

        render.Params.frame = render.Time;

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