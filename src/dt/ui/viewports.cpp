#include "viewports.h"

#include "dt/logging.h"
#include "dt/core/world.h"

#include "pxr/usd/usd/prim.h"
#include "pxr/usd/usd/primRange.h"
#include "pxr/usd/usdGeom/camera.h"
#include "pxr/imaging/hgiGL/texture.h"

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
    _Names[0] = "USD Viewport";
    _Renders[0].Reset();

    for (size_t i = 1; i < _Renders.size(); i++)
    {
        _Renders[i].Reset();
        _Names[i] = std::format("USD Viewport (#{})", i + 1);
    }

    log::debug("Initialized {} USD viewports", _Renders.size());
}

void dt::Viewports::DrawAll()
{
    for (size_t i = 0; i < _Renders.size(); i++)
    {
        Render &render = _Renders[i];
        __draw_render(render, _Names[i].c_str());
        __draw_static_render_controller();
        __draw_static_render_parameter();
        __refresh_camera_paths();
    }
}

void dt::Viewports::Reset()
{
    for (auto &render : _Renders)
        render.Reset();
}

void dt::Viewports::__draw_render(Render &render, const char *name)
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{});

    if (ImGui::Begin(name, nullptr, ImGuiWindowFlags_MenuBar))
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
                    // glBindTexture(GLrender.TEXTURE_2D, __get_texture());
                    // glPixelStorei(GLrender.PACK_ALIGNMENT, 1);
                    // glGetTexImage(GLrender.TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, handler->Pixels.data());
                    // glBindTexture(GLrender.TEXTURE_2D, 0);
                    // FileDialog::Show<FileDialog::Mode::SAVE>(std::move(handler), FileDialog::IMAGErender.FILTER);
                }

                if (ImGui::InputInt2("Resolution", &render.size[0]))
                    render.UpdateSize();

                auto get_path = [](void *user_data, int idx) -> const char *
                {
                    const auto *paths = (pxr::SdfPath *)(user_data);
                    return paths[idx].GetText();
                };

                if (ImGui::Combo("Camera Path", &_Active, get_path, (void *)_Camera_paths.data(), _Camera_paths.size()))
                {
                    render.free_camera = false;
                }

                ImGui::Checkbox("Free Camera", &render.free_camera);
                ImGui::EndMenu();
            }
            __draw_render_menu(render);
            ImGui::EndMenuBar();
        }

        ImVec2 size(render.size[0], render.size[1]);
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
        // render.Capture_input = ImGui::IsItemHovered() && ImGui::IsMouseClicked(0);
    }
    else
    {
        ImGui::PopStyleVar();
    }
    ImGui::End();
}

void dt::Viewports::__draw_render_menu(Render &render)
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
            render.Time = render.Latest;
            render.Latest = World::GetTime();
        }

        if (ImGui::SliderFloat("Time Code", &render.Time, 0.f, render.Latest))
        {
            render.Live = false;
        }

        render.Params.frame = render.Time;

        ImGui::EndMenu();
    }
}

void dt::Viewports::__draw_static_render_controller()
{
    ImGui::Begin("Controller");
    ImGui::InputFloat("Speed", &Render::Speed, 1.f, 20.f, "%.0f");
    ImGui::InputFloat("Sensitivity", &Render::Sense, 1.f, 20.f, "%.0f");
    ImGui::End();
}

void dt::Viewports::__draw_static_render_parameter()
{
    if (ImGui::Begin("Parameter"))
    {
        ImGui::Checkbox("Lighting", &Render::Params.enableLighting);
        ImGui::Checkbox("Scene Lights", &Render::Params.enableSceneLights);
        ImGui::Checkbox("Scene Materials", &Render::Params.enableSceneMaterials);
        ImGui::Checkbox("Show Guides", &Render::Params.showGuides);
        ImGui::Checkbox("Show Proxy", &Render::Params.showProxy);
        ImGui::Checkbox("Show Render", &Render::Params.showRender);
        ImGui::Checkbox("Force Refresh", &Render::Params.forceRefresh);
        ImGui::Checkbox("Sample Alpha to Coverage", &Render::Params.enableSampleAlphaToCoverage);
        ImGui::Checkbox("Gamma Correct Colors", &Render::Params.gammaCorrectColors);
        ImGui::ColorEdit4("Clear Color", &Render::Params.clearColor[0]);
        ImGui::SliderFloat("Complexity", &Render::Params.complexity, 1.f, 1.5f, "%.1f");
        ImGui::Combo("Draw Mode", (int *)&Render::Params.drawMode, DRAW_MODES, std::size(DRAW_MODES));
        ImGui::Combo("Cull Style", (int *)&Render::Params.cullStyle, CULL_STYLES, std::size(CULL_STYLES));
        ImGui::Checkbox("Live Playback", &Render::Live);
    }

    if (Render::Live)
    {
        Render::Time = Render::Latest;
        Render::Latest = World::GetTime();
    }

    if (ImGui::SliderFloat("Time Code", &Render::Time, 0.f, Render::Latest))
    {
        Render::Live = false;
    }

    Render::Params.frame = Render::Time;

    ImGui::End();
}

// TODO: Use events instaed of checking every frame
void dt::Viewports::__refresh_camera_paths()
{
    unsigned hits = 0;

    auto [stage, _] = World::GetStagePermit();

    for (const pxr::UsdPrim &prim : stage->Traverse())
    {
        if (prim.IsA<pxr::UsdGeomCamera>())
        {
            if (hits >= _Camera_paths.size())
            {
                _Camera_paths.emplace_back(prim.GetPath());
            }
            else if (prim.GetPath() != _Camera_paths[hits])
            {
                _Camera_paths[hits] = prim.GetPath();
            }
            hits++;
        }
    }
}