#include "multi_viewport.h"

#include "nexus/core/world.h"
#include "nexus/event/event_client.h"
#include "nexus/event/scene_reset_event.h"
#include "nexus/event/viewport_capture_event.h"
#include "nexus/view/filedialog.h"

#include "pxr/imaging/hgiGL/texture.h"
#include "pxr/usd/usd/prim.h"
#include "pxr/usd/usd/primRange.h"
#include "pxr/usd/usdGeom/camera.h"

#include "SDL3/SDL_opengl.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
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

Nexus::MultiViewport::MultiViewport()
{
    LOG_BASIC("Constructing with {} renderers...", m_Renders.size());

    m_RenderNames[0] = "USD Viewport";

    for (auto i = 1ull; i < m_Renders.size(); i++)
        m_RenderNames[i] = std::format("USD Viewport (#{})", i + 1);

    EventClient::On<SceneResetEvent>(
        [this](const SceneResetEvent &)
        {
            _refresh_camera_paths();
            m_Renders[0].reset();
            m_Active = 1;
        });
}

void Nexus::MultiViewport::start_engine()
{
    m_Renders[0].reset();
    LOG_EVENT("Started render engine");
}

void Nexus::MultiViewport::stop_engine()
{
    for (auto &render : m_Renders)
        render.delete_engine();

    LOG_EVENT("Stopped render engine");
}

void Nexus::MultiViewport::draw()
{
    for (std::size_t i = 0; i < m_Active; ++i)
        _draw_render(i);

    _draw_main_menu();
    _draw_static_render_controller();
    _draw_static_render_parameter();
}

void Nexus::MultiViewport::_draw_main_menu()
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("MultiViewport"))
        {
            ImGui::Text("Active: %zu/%zu", m_Active, m_Renders.size());

            if (ImGui::Button("Add"))
            {
                if (m_Active < m_Renders.size())
                {
                    m_Active++;
                    m_Renders[m_Active - 1].reset();
                    LOG_EVENT("Added {}", m_RenderNames[m_Active - 1]);
                }
                else
                {
                    LOG_ALERT("Reached maximum number of viewports ({})!", m_Renders.size());
                }
            }

            ImGui::SameLine();

            if (ImGui::Button("Remove"))
            {
                if (m_Active > 1)
                {
                    m_Active--;
                    LOG_EVENT("Removed {}", m_RenderNames[m_Active]);
                }
                else
                {
                    LOG_ALERT("Must have at least one render viewport!");
                }
            }

            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

void Nexus::MultiViewport::_draw_render(size_t index)
{
    Render &render = m_Renders[index];

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2());

    if (ImGui::Begin(m_RenderNames[index].c_str(), nullptr, ImGuiWindowFlags_MenuBar))
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

                    FileDialog::Show<FileDialog::Mode::SAVE, FileDialog::Thread::SDL>(
                        [pixels = std::move(pixels), width, height](std::string path, int filter)
                        {
                            const auto f = std::format("{}.{}", path, FileDialog::IMAGE_FILTER[filter].pattern);

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
                                LOG_ALERT("Saving as HDR is not supported yet!");
                                // TODO: Casting char to float is not a good idea
                                // if (!stbi_write_hdr(f.c_str(), Width, Height, STRIDE, reinterpret_cast<float *>(Pixels.data())));
                                //     LOG_ERROR("Error saving as HDR");
                                break;
                            default:
                                LOG_ERROR("Unknown image format (filter={})", filter);
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

                if (ImGui::Combo("Camera Path", (int *)&m_CameraIndices[index], get_path, (void *)m_CameraPaths.data(), m_CameraPaths.size()))
                {
                    LOG_EVENT("Changed camera path for {} to {}",
                              m_RenderNames[index].c_str(),
                              m_CameraPaths[m_CameraIndices[index]].GetText());
                    render.CameraPath = m_CameraPaths[m_CameraIndices[index]];
                    render.FreeCamera = false;
                }

                ImGui::Checkbox("Toggle Free Camera", &render.FreeCamera);
                ImGui::EndMenu();
            }
            _draw_render_menu(render);
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
            m_Captured = index;

            if (!render.FreeCamera)
                render.transform_to_camera();

            render.FreeCamera = true;
            EventClient::Send<ViewportCaptureEvent>();
        }
    }
    else
    {
        ImGui::PopStyleVar();
    }
    ImGui::End();
}

void Nexus::MultiViewport::_draw_render_menu(Render &render)
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

void Nexus::MultiViewport::_draw_static_render_controller()
{
    ImGui::Begin("Controller");
    ImGui::InputFloat("Speed", &Render::SPEED, 1.f, 20.f, "%.0f");
    ImGui::InputFloat("Sensitivity", &Render::SENSITIVITY, 1.f, 20.f, "%.0f");
    ImGui::End();
}

void Nexus::MultiViewport::_draw_static_render_parameter()
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

void Nexus::MultiViewport::_refresh_camera_paths()
{
    unsigned hits = 0;

    auto [stage, lock] = World::GetStageReadAccess();

    for (const pxr::UsdPrim &prim : stage->Traverse())
    {
        if (prim.IsA<pxr::UsdGeomCamera>())
        {
            if (hits >= m_CameraPaths.size())
            {
                m_CameraPaths.emplace_back(prim.GetPath());
            }
            else if (prim.GetPath() != m_CameraPaths[hits])
            {
                m_CameraPaths[hits] = prim.GetPath();
            }
            hits++;
        }
    }
    LOG_EVENT("Refreshed camera paths, cached {} in total", hits);
}