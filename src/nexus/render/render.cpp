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
        m_Engine->SetCameraPath(CameraPath);
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

    if (!handle)
        throw exception("AOV texture handle is null");

    const auto *texture = static_cast<pxr::HgiGLTexture *>(handle.Get());

    if (!texture)
        throw exception("HGI color texture is null");

    if (texture->GetTextureId() == 0)
        throw exception("OpenGL texture is invalid");

    return texture->GetTextureId();
}

void Nexus::Render::reset()
{
    this->delete_engine();
    m_Engine = new (m_Raw) Engine{};
    m_Engine->SetEnablePresentation(false);

    if (!m_Engine->SetRendererAov(pxr::HdAovTokens->color))
        throw exception("Error setting AOV to color");

    this->update_size();
}

void Nexus::Render::update_size()
{
    const pxr::GfRect2i dataWindow(pxr::GfVec2i(0), Size);
    m_Engine->SetFraming(pxr::CameraUtilFraming(dataWindow));
    m_Engine->SetRenderBufferSize(Size);
}

void Nexus::Render::delete_engine()
{
    if (m_Engine)
    {
        m_Engine->~Engine();
        m_Engine = nullptr;
    }
}

void Nexus::Render::transform_to_camera()
{
    auto [stage, lock] = World::GetStageReadAccess();
    pxr::UsdGeomCamera c(stage->GetPrimAtPath(CameraPath));
    this->transform_from(c.GetCamera(this->Params.frame));
}