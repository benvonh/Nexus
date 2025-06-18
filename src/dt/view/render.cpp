#include "dt/base.hpp"
#include "dt/view/filedialog.hpp"
#include "dt/view/render.hpp"
#include "SDL3/SDL_dialog.h"
#include "SDL3/SDL_opengl.h"
#include "imgui.h"
#include "pxr/imaging/hgiGL/texture.h"
#include "stb_image_write.h"

dt::view::Render::Render(const std::string &id) : name(id)
{
  _Initialize_Engine();
}

dt::view::Render::Render(Render &&other)
    : Controller(std::move(other)), name(std::move(other.name))
{
  _size = other._size;
  _pathIndex = other._pathIndex;
  _freeCamera = other._freeCamera;
  _Initialize_Engine();
}

unsigned dt::view::Render::operator()(pxr::UsdStageRefPtr &stage)
{
  if (_freeCamera || _pathIndex >= Render::paths.size())
  {
    const pxr::GfFrustum f = Controller::Frustum();
    _engine.SetCameraState(f.ComputeViewMatrix(), f.ComputeProjectionMatrix());
  }
  else
  {
    _engine.SetCameraPath(Render::paths[_pathIndex]);
  }
  _engine.Render(stage->GetPseudoRoot(), _Params());
  return _GetTexture();
}

void dt::view::Render::SetToFreeCamera(pxr::UsdStageRefPtr &stage)
{
  if (!_freeCamera)
  {
    pxr::UsdGeomCamera schema(stage->GetPrimAtPath(Render::paths[_pathIndex]));
    Controller::TransformFrom(schema.GetCamera(Render::Params().frame));
  }
  _freeCamera = true;
}

void dt::view::Render::Capture()
{
  std::vector<unsigned char> pixels(_GetWidth() * _GetHeight() * 4);
  glBindTexture(GL_TEXTURE_2D, _GetTexture());
  glPixelStorei(GL_PACK_ALIGNMENT, 1);
  glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
  stbi_flip_vertically_on_write(1);
  stbi_write_png(data.file.c_str(), _GetWidth(), _GetHeight(), 4, pixels.data(), _GetWidth() * 4);
  glBindTexture(GL_TEXTURE_2D, 0);
}

void dt::view::Render::Draw()
{
  if (ImGui::BeginMenuBar())
  {
    if (ImGui::BeginMenu("Settings"))
    {
      if (ImGui::InputInt2("Resolution", &_size[0]))
      {
        const pxr::GfRect2i dataWindow(pxr::GfVec2i(0), _size);
        _engine.SetFraming(pxr::CameraUtilFraming(dataWindow));
        _engine.SetRenderBufferSize(_size);
      }
      if (_pathIndex >= _paths.size())
      {
        _pathIndex = 0;
      }
      if (ImGui::Combo("Camera Path", &_pathIndex, PathGetter, (void *)paths.data(), paths.size()))
      {
        _freeCamera = false;
      }
      ImGui::Checkbox("Free Camera", &_freeCamera);
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Capture"))
    {
      if (ImGui::Button("Save to file"))
      {
        FileDialog::Show<FileDialog::Mode::SAVE>(_window, &data, FileDialog::usdFilter);
      }
      ImGui::EndMenu();
    }
    ImGui::EndMenuBar();
  }
  ImVec2 size(render.GetWidth(), render.GetHeight());
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
  ImGui::Image(render(permit.stage), size, ImVec2(0, 1), ImVec2(1, 0));
}

unsigned dt::view::Render::GetTexture()
{
  const auto handle = _engine.GetAovTexture(pxr::HdAovTokens->color);

  if (!handle)
    throw exception("AOV texture handle is null");

  const auto *texture = static_cast<pxr::HgiGLTexture *>(handle.Get());

  if (!texture)
    throw exception("HGI color texture is null");

  if (texture->GetTextureId() == 0)
    throw exception("OpenGL texture is invalid");

  return texture->GetTextureId();
}

void dt::view::Render::_Initialize_Engine()
{
  if (!_engine.SetRendererAov(pxr::HdAovTokens->color))
    throw exception("Error setting AOV to color");

  const pxr::GfRect2i dataWindow(pxr::GfVec2i(0), _size);
  _engine.SetFraming(pxr::CameraUtilFraming(dataWindow));
  _engine.SetRenderBufferSize(_size);
  _engine.SetEnablePresentation(false);
}