#include "digital_twin/base.hpp"
#include "dt/view/render.hpp"
#include "SDL3/SDL_dialog.h"
#include "imgui.h"
#include "pxr/imaging/hgiGL/texture.h"

dt::view::Render::Render(const std::string &id) : name(id)
{
  this->InitializeEngine();
}

dt::view::Render::Render(Render &&other)
    : Controller(std::move(other)), name(std::move(other.name))
{
  this->size = other.size;
  this->pathIndex = other.pathIndex;
  this->freeCamera = other.freeCamera;
  this->InitializeEngine();
}

unsigned dt::view::Render::operator()(pxr::UsdStageRefPtr &stage)
{
  if (this->freeCamera || this->pathIndex >= Render::paths.size())
  {
    const pxr::GfFrustum f = Controller::Frustum();
    this->engine.SetCameraState(f.ComputeViewMatrix(), f.ComputeProjectionMatrix());
  }
  else
  {
    this->engine.SetCameraPath(Render::paths[this->pathIndex]);
  }
  this->engine.Render(stage->GetPseudoRoot(), this->Params());
  return this->GetTexture();
}

void dt::view::Render::SetToFreeCamera(pxr::UsdStageRefPtr &stage)
{
  if (!this->freeCamera)
  {
    pxr::UsdGeomCamera schema(stage->GetPrimAtPath(Render::paths[this->pathIndex]));
    Controller::TransformFrom(schema.GetCamera(Render::Params().frame));
  }
  this->freeCamera = true;
}

void dt::view::Render::Draw()
{
  if (ImGui::BeginMenuBar())
  {
    if (ImGui::BeginMenu("Settings"))
    {
      if (ImGui::InputInt2("Resolution", &this->size[0]))
      {
        const pxr::GfRect2i dataWindow(pxr::GfVec2i(0), this->size);
        this->engine.SetFraming(pxr::CameraUtilFraming(dataWindow));
        this->engine.SetRenderBufferSize(this->size);
      }
      if (this->pathIndex >= this->paths.size())
      {
        this->pathIndex = 0;
      }
      if (ImGui::Combo("Camera Path", &this->pathIndex, PathGetter, (void *)paths.data(), paths.size()))
      {
        this->freeCamera = false;
      }
      ImGui::Checkbox("Free Camera", &this->freeCamera);
      ImGui::EndMenu();
    }
    ImGui::EndMenuBar();
  }
}

unsigned dt::view::Render::GetTexture()
{
  const auto handle = this->engine.GetAovTexture(pxr::HdAovTokens->color);

  if (!handle)
    throw exception("AOV texture handle is null");

  const auto *texture = static_cast<pxr::HgiGLTexture *>(handle.Get());

  if (!texture)
    throw exception("HGI color texture is null");

  if (texture->GetTextureId() == 0)
    throw exception("OpenGL texture is invalid");

  return texture->GetTextureId();
}

void dt::view::Render::InitializeEngine()
{
  if (!this->engine.SetRendererAov(pxr::HdAovTokens->color))
    throw exception("Error setting AOV to color");

  const pxr::GfRect2i dataWindow(pxr::GfVec2i(0), this->size);
  this->engine.SetFraming(pxr::CameraUtilFraming(dataWindow));
  this->engine.SetRenderBufferSize(this->size);
  this->engine.SetEnablePresentation(false);
}