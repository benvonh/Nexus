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
    const pxr::GfFrustum f = this->camera.GetFrustum();

    _Engine->SetCameraState(f.ComputeViewMatrix(), f.ComputeProjectionMatrix());
    {
        auto [stage, _] = World::GetStagePermit();
        _Engine->Render(stage->GetPseudoRoot(), this->params);
    }
    return __get_texture();
}

unsigned dt::Render::operator()(const pxr::SdfPath &path)
{
    _Engine->SetCameraPath(path);
    {
        auto [stage, _] = World::GetStagePermit();
        _Engine->Render(stage->GetPseudoRoot(), this->params);
    }
    return __get_texture();
}

void dt::Render::Reset()
{
    _Engine.emplace();

    if (!_Engine->SetRendererAov(pxr::HdAovTokens->color))
        throw exception("Error setting AOV to color");

    const pxr::GfRect2i dataWindow(pxr::GfVec2i(0), this->size);
    _Engine->SetFraming(pxr::CameraUtilFraming(dataWindow));
    _Engine->SetRenderBufferSize(this->size);
    _Engine->SetEnablePresentation(false);
}

void dt::Render::UpdateSize()
{
    const pxr::GfRect2i dataWindow(pxr::GfVec2i(0), this->size);
    _Engine->SetFraming(pxr::CameraUtilFraming(dataWindow));
    _Engine->SetRenderBufferSize(this->size);
}

void dt::Render::EnableFreeCamera(const pxr::SdfPath &path)
{
    if (!this->free_camera)
    {
        auto [stage, _] = World::GetStagePermit();
        pxr::UsdGeomCamera cam(stage->GetPrimAtPath(path));
        this->TransformFrom(cam.GetCamera(this->params.frame));
    }
    this->free_camera = true;
}

void dt::Render::DisableFreeCamera()
{
    this->free_camera = false;
}

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