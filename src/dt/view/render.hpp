#pragma once
#include "pxr/base/gf/frustum.h"
#include "pxr/base/gf/vec2i.h"
#include "pxr/usd/sdf/path.h"
#include "pxr/usdImaging/usdImagingGL/engine.h"
#include "pxr/usdImaging/usdImagingGL/renderParams.h"
#include <memory>

namespace dt
{
namespace view
{
class Render
{
  using Size = pxr::GfVec2i;
  using Engine = pxr::UsdImagingGLEngine;
  using Params = pxr::UsdImagingGLRenderParams;
  
  static constexpr int DEFAULT_WIDTH = 1280;
  static constexpr int DEFAULT_HEIGHT = 720;

public:
  static inline const char *DRAW_MODES[] = {
    "Points",
    "Wireframe",
    "Wireframe on Surface",
    "Shaded Flat",
    "Shaded Smooth",
    "Geometry Only",
    "Geometry Flat",
    "Geometry Smooth"
  };

  static inline const char *CULL_STYLES[] = {
    "No Opinion",
    "Nothing",
    "Back",
    "Front",
    "Back Unless Double-Sided"
  };

  static constexpr auto DRAW_MODES_SIZE = std::size(DRAW_MODES);
  static constexpr auto CULL_STYLES_SIZE = std::size(CULL_STYLES);

  Size size;
  Params params;
  bool domeLight;

  Render() { this->Reset(); }
  ~Render() = default;
  Render(Render &&) = delete;
  Render(const Render &) = delete;
  Render &operator=(Render &&) = delete;
  Render &operator=(const Render &) = delete;

  void Reset();
  void UpdateSize();
  void UpdateDomeLight();
  void UpdateCameraPath(const pxr::SdfPath &path);
  void UpdateCameraState(const pxr::GfFrustum &frustum);

  unsigned long long operator()();

private:
  std::unique_ptr<Engine> engine;
};
} // namespace view
} // namespace dt