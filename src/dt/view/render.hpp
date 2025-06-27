#pragma once
#include "dt/view/controller.hpp"
#include "dt/view/parameter.hpp"
#include "pxr/base/gf/vec2i.h"
#include "pxr/usd/sdf/path.h"
#include "pxr/usd/usd/primRange.h"
#include "pxr/usdImaging/usdImagingGL/engine.h"
#include <string>

namespace dt
{
  namespace view
  {
    class Render : public Controller, Parameter
    {
    public:
            Render(const std::string &id);
      ~Render() = default;
      Render(Render &&);

      Render(const Render &) = delete;
      Render &operator=(Render &&) = delete;
      Render &operator=(const Render &) = delete;
      unsigned operator()(pxr::UsdStageRefPtr &stage);
      void SetToFreeCamera(pxr::UsdStageRefPtr &stage);
      void Capture();
      void Draw();

    private:
      unsigned _Texture();
      void _Initialize_Engine();

      static const char *PathGetter(void *user_data, int idx)
      {
        const pxr::SdfPath *paths = (pxr::SdfPath *)(user_data);
        return paths[idx].GetText();
      }

      int _pathIndex = 0;
      bool _freeCamera = true;
      pxr::GfVec2i _size = {1280, 720};
      pxr::UsdImagingGLEngine _engine;

      const std::string _name;
    };
  }
}