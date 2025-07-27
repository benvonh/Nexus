#include "render.h"

#include "nexus/core/world.h"
#include "nexus/exception.h"

#include "pxr/base/gf/rect2i.h"
#include "pxr/base/gf/vec2i.h"
#include "pxr/imaging/hgiGL/texture.h"
#include "pxr/usd/usd/prim.h"
#include "pxr/usd/usdGeom/camera.h"

unsigned Nexus::Render::operator()()
{
    if (FreeCamera)
    {
        const auto f = this->Camera.GetFrustum();
        m_Engine->SetCameraState(f.ComputeViewMatrix(), f.ComputeProjectionMatrix());
    }
    else
    {
        m_Engine->SetCameraPath(this->CameraPath);
    }
    // Storm
    {
        auto [stage, lock] = World::GetStageReadAccess();
        m_Engine->Render(stage->GetPseudoRoot(), this->Params);
    }
    return this->get_texture();
}

unsigned Nexus::Render::get_texture()
{
    const auto handle = m_Engine->GetAovTexture(pxr::HdAovTokens->color);

    if (!handle) [[unlikely]]
        throw exception("AOV texture handle is null");

    const auto *texture = (pxr::HgiGLTexture *)(handle.Get());

    if (!texture) [[unlikely]]
        throw exception("HGI color texture is null");

    if (texture->GetTextureId() == 0) [[unlikely]]
        throw exception("OpenGL texture is invalid");

    return texture->GetTextureId();
}

void Nexus::Render::reset()
{
    this->delete_engine();
    m_Engine = new (m_EngineMemorySpace) Engine{};
    m_Engine->SetEnablePresentation(false);

    if (!m_Engine->SetRendererAov(pxr::HdAovTokens->color))
        throw exception("Tried to set renderer AOV to color");

    this->update_size();

    LOG_EVENT("Engine has been reset");
}

void Nexus::Render::update_size()
{
    const pxr::GfRect2i dataWindow(pxr::GfVec2i(0), this->Size);
    m_Engine->SetFraming(pxr::CameraUtilFraming(dataWindow));
    m_Engine->SetRenderBufferSize(this->Size);
    LOG_EVENT("Size updated to {}x{}", this->Size[0], this->Size[1]);
}

void Nexus::Render::delete_engine()
{
    if (m_Engine)
    {
        m_Engine->~Engine();
        m_Engine = nullptr;
        LOG_EVENT("Engine deleted");
    }
}

void Nexus::Render::transform_to_camera()
{
    auto [stage, lock] = World::GetStageReadAccess();
    pxr::UsdGeomCamera cam(stage->GetPrimAtPath(this->CameraPath));
    this->transform_from(cam.GetCamera(this->Params.frame));
    LOG_EVENT("Transformation at {}", this->CameraPath.GetText());
}