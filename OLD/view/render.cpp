#include "dt/view/render.hpp"
#include "dt/util.hpp"
#include "SDL3/SDL_dialog.h"
#include "SDL3/SDL_opengl.h"
#include "imgui.h"
#include "pxr/imaging/hgiGL/texture.h"
#include "stb_image_write.h"

void dt::view::Render::HandleFile::operator()(const std::string &path)
{
  stbi_flip_vertically_on_write(1);

  if (!stbi_write_png(path.c_str(), width, height, 4, pixels.data(), width * 4))
    throw exception("Error saving render ({}x{}) to file at '{}'", width, height, path);
}

void dt::view::Render::Reset()
{
  _engine.emplace();

  if (!_engine.SetRendererAov(pxr::HdAovTokens->color))
    throw exception("Error setting AOV to color");

  const pxr::GfRect2i dataWindow(pxr::GfVec2i(0), _size);
  _engine.SetFraming(pxr::CameraUtilFraming(dataWindow));
  _engine.SetRenderBufferSize(_size);
  _engine.SetEnablePresentation(false);
}

unsigned dt::view::Render::operator()()
{
  auto permit = scene::Manager::GetStagePermit();

  if (_freeCamera || _sdfPathIndex >= _sdfPaths.size())
  {
    const pxr::GfFrustum f = Controller::Frustum();
    _engine.SetCameraState(f.ComputeViewMatrix(), f.ComputeProjectionMatrix());
  }
  else
  {
    _engine.SetCameraPath(_sdfPaths[_sdfPathIndex]);
  }
  _engine.Render(permit.stage->GetPseudoRoot(), Parameter::Params());
  return _Get_Texture();
}

void dt::view::Render::SetToFreeCamera()
{
  auto permit = scene::Manager::GetStagePermit();

  if (!_freeCamera)
  {
    pxr::UsdGeomCamera c(permit.stage->GetPrimAtPath(_sdfPaths[_sdfPathIndex]));
    Controller::TransformFrom(c.GetCamera(Parameter::Params().frame));
  }
  _freeCamera = true;
}

void dt::view::Render::Capture()
{
  HandleFile handle(_size[0] * _size[1] * 4);
  glBindTexture(GL_TEXTURE_2D, _Get_Texture());
  glPixelStorei(GL_PACK_ALIGNMENT, 1);
  glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, handle.pixels.data());
  glBindTexture(GL_TEXTURE_2D, 0);
  FileDialog::Show<FileDialog::Mode::SAVE>(handle&, FileDialog::usd_filter);
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
      if (ImGui::Combo("Camera Path", &_pathIndex, path_getter, (void *)paths.data(), paths.size()))
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
  ImGui::Image(render(), size, ImVec2(0, 1), ImVec2(1, 0));
}

unsigned dt::view::Render::_Get_Texture()
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