#include "render.h"

#include "dt/core/world.h"
#include "dt/exception.h"
#include "dt/logging.h"

#include "pxr/base/gf/rect2i.h"
#include "pxr/base/gf/vec2i.h"
#include "pxr/imaging/hgiGL/texture.h"
#include "pxr/usd/usd/prim.h"
#include "pxr/usd/usdGeom/camera.h"

// TODO
#include "SDL3/SDL_opengl.h"

// TODO
#include "stb_image_write.h"

unsigned dt::Render::operator()()
{
    if (FreeCamera)
    {
        const auto f = this->Camera.GetFrustum();
        M_Engine->SetCameraState(f.ComputeViewMatrix(), f.ComputeProjectionMatrix());
    }
    else
    {
        M_Engine->SetCameraPath(M_CameraPath);
    }
    {
        auto [stage, _] = World::GetStagePermit();
        M_Engine->Render(stage->GetPseudoRoot(), this->Params);
    }
    return this->get_texture();
}

unsigned dt::Render::get_texture()
{
    const auto handle = M_Engine->GetAovTexture(pxr::HdAovTokens->color);

    if (!handle)
        throw exception("AOV texture handle is null");

    const auto *texture = static_cast<pxr::HgiGLTexture *>(handle.Get());

    if (!texture)
        throw exception("HGI color texture is null");

    if (texture->GetTextureId() == 0)
        throw exception("OpenGL texture is invalid");

    return texture->GetTextureId();
}

void dt::Render::reset()
{
    M_Engine.emplace();
    M_Engine->SetEnablePresentation(false);

    if (!M_Engine->SetRendererAov(pxr::HdAovTokens->color))
        throw exception("Error setting AOV to color");

    this->update_size();
}

void dt::Render::update_size()
{
    const pxr::GfRect2i dataWindow(pxr::GfVec2i(0), Size);
    M_Engine->SetFraming(pxr::CameraUtilFraming(dataWindow));
    M_Engine->SetRenderBufferSize(Size);
}

void dt::Render::transform_to_camera()
{
    auto [stage, _] = World::GetStagePermit();
    pxr::UsdGeomCamera c(stage->GetPrimAtPath(M_CameraPath));
    this->transform_from(c.GetCamera(this->Params.frame));
}

void dt::Render::set_camera_path(const pxr::SdfPath &path)
{
    M_CameraPath = path;
}