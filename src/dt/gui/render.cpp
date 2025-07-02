#include "render.h"

#include "dt/usd/world.h"
#include "dt/exception.h"
#include "dt/logging.h"

#include "pxr/usd/usd/prim.h"
#include "pxr/usd/usd/primRange.h"
#include "pxr/usd/usdGeom/camera.h"
#include "pxr/imaging/hgiGL/texture.h"

#include "imgui.h"

#include <format>
#include <string_view>

/*============================================================================*/
// TODO: Is this gonna work? lol
static std::string_view generate_name()
{
    static unsigned calls = 0;

    std::string_view name;

    if (calls++ > 0)
        name = std::format("USD Viewport (#{})", calls);
    else
        name = "USD Viewport";

    return name;
}

/*============================================================================*/
static const char *get_path(void *user_data, int idx)
{
    const auto *paths = (pxr::SdfPath *)(user_data);
    return paths[idx].GetText();
}

/*============================================================================*/
dt::Render::Render() : _NAME(generate_name()) {}

/*============================================================================*/
bool dt::Render::draw()
{
    ImGui::Begin(_NAME.data(), nullptr, ImGuiWindowFlags_MenuBar);

    /*****************
     * SHOW MENU BAR *
     *****************/
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Settings"))
        {
            if (ImGui::InputInt2("Resolution", &_Size[0]))
            {
                const pxr::GfRect2i dataWindow(pxr::GfVec2i(0), _Size);
                _Engine->SetFraming(pxr::CameraUtilFraming(dataWindow));
                _Engine->SetRenderBufferSize(_Size);
            }
            if (ImGui::Combo("Camera Path", &_Path_index, get_path, (void *)__Paths, __Cached_paths))
            {
                _Free_camera = false;
            }
            ImGui::Checkbox("Free Camera", &_Free_camera);
            ImGui::EndMenu();
        }
        Controller::draw();
        Parameter::draw();
        ImGui::EndMenuBar();
    }

    /****************
     * RENDER SCENE *
     ****************/
    if (_Free_camera || _Path_index >= __Cached_paths)
    {
        const pxr::GfFrustum f = Controller::get_frustum();
        _Engine->SetCameraState(f.ComputeViewMatrix(), f.ComputeProjectionMatrix());
    }
    else
    {
        _Engine->SetCameraPath(__Paths[_Path_index]);
    }
    {
        auto permit = World::get_stage_permit();

        _Engine->Render(permit.Stage->GetPseudoRoot(), Parameter::get_params());
    }

    /******************
     * DISPLAY RENDER *
     ******************/
    ImVec2 size(_Size[0], _Size[1]);
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
    ImGui::Image(__get_texture(), size, ImVec2(0, 1), ImVec2(1, 0));

    bool clicked = ImGui::IsItemHovered() && ImGui::IsMouseClicked(0);

    ImGui::End();

    return clicked;
}

/*============================================================================*/
void dt::Render::reset()
{
    _Engine.emplace();

    if (!_Engine->SetRendererAov(pxr::HdAovTokens->color))
        throw exception("Error setting AOV to color");

    const pxr::GfRect2i dataWindow(pxr::GfVec2i(0), _Size);
    _Engine->SetFraming(pxr::CameraUtilFraming(dataWindow));
    _Engine->SetRenderBufferSize(_Size);
    _Engine->SetEnablePresentation(false);
}

/*============================================================================*/
void dt::Render::enable_free_camera()
{
    if (!_Free_camera)
    {
        auto permit = World::get_stage_permit();
        pxr::UsdGeomCamera c(permit.Stage->GetPrimAtPath(__Paths[_Path_index]));
        Controller::transform_from(c.GetCamera(Parameter::get_params().frame));
    }
    _Free_camera = true;
}

/*============================================================================*/
unsigned dt::Render::__get_texture()
{
    const auto handle = _Engine->GetAovTexture(pxr::HdAovTokens->color);

    if (!handle)
        throw exception("AOV texture handle is null");

    const auto *texture = static_cast<pxr::HgiGLTexture *>(handle.Get());

    if (!texture)
        throw exception("HGI color texture is null");

    if (texture->GetTextureId() == 0)
        throw exception("OpenGL texture is invalid");

    return texture->GetTextureId();
}

/*============================================================================*/
void dt::Render::CachePaths()
{
    static bool logMaxHit = false;

    static size_t hits = 0;

    auto premit = World::get_stage_permit();

    for (const pxr::UsdPrim &prim : premit.Stage->Traverse())
    {
        if (prim.IsA<pxr::UsdGeomCamera>())
        {
            if (hits == __CACHE_SIZE)
            {
                if (!logMaxHit)
                {
                    logMaxHit = true;
                    log::alert("Hit maximum cache size of {}", __CACHE_SIZE);
                }
                break;
            }
            if (prim.GetPath() != __Paths[hits])
            {
                __Paths[hits] = prim.GetPath();
            }
            hits++;
        }
    }

    if (hits != __CACHE_SIZE)
        logMaxHit = false;

    __Cached_paths = hits;
}