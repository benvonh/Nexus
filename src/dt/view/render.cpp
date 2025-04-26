#include "dt/view/render.hpp"
#include "dt/scene/manager.hpp"
#include "pxr/imaging/hgiGL/texture.h"

namespace dt
{
namespace view
{
void Render::Reset()
{
  this->domeLight = true;

  this->size = Size(DEFAULT_WIDTH, DEFAULT_HEIGHT);

  this->params = Params();
  this->params.enableLighting = true;
  this->params.enableSampleAlphaToCoverage = true;
  this->params.enableSceneLights = true;
  this->params.enableSceneMaterials = true;
  this->params.forceRefresh = false;
  this->params.gammaCorrectColors = false;
  this->params.showGuides = false;
  this->params.showProxy = false;
  this->params.showRender = false;

  this->engine = std::make_unique<Engine>();
  this->engine->SetEnablePresentation(false);

  if (!this->engine->SetRendererAov(pxr::HdAovTokens->color))
    throw std::runtime_error("AOV is not color");

  // Call again
  this->UpdateSize();
}

void Render::UpdateSize()
{
  const pxr::GfRect2i dataWindow(pxr::GfVec2i(0), this->size);
  this->engine->SetFraming(pxr::CameraUtilFraming(dataWindow));
  this->engine->SetRenderBufferSize(this->size);
}

void Render::UpdateDomeLight()
{
  const auto &token = pxr::HdRenderSettingsTokens->domeLightCameraVisibility;
  this->engine->SetRendererSetting(token, pxr::VtValue(this->domeLight));
}

void Render::UpdateCameraPath(const pxr::SdfPath &path)
{
  this->engine->SetCameraPath(path);
}

void Render::UpdateCameraState(const pxr::GfFrustum &frustum)
{
  this->engine->SetCameraState(frustum.ComputeViewMatrix(),
                               frustum.ComputeProjectionMatrix());
}

unsigned long long Render::operator()()
{
  {
    auto permit = scene::Manager::GetStagePermit();

    if (!permit.stage)
      throw std::runtime_error("USD stage is null");

    this->engine->Render(permit.stage->GetPseudoRoot(), this->params);
  }

  auto handle = this->engine->GetAovTexture(pxr::HdAovTokens->color);

  if (!handle)
    throw std::runtime_error("AOV texture handle is null");

  auto *texture = static_cast<pxr::HgiGLTexture *>(handle.Get());

  if (!texture)
    throw std::runtime_error("HGI color texture is null");

  if (texture->GetTextureId() == 0)
    throw std::runtime_error("OpenGL texture is invalid");
  
  return texture->GetTextureId();
}
} // namespace dt
} // namespace dt