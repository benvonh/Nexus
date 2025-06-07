#pragma once
#include "dt/view/controller.hpp"
#include "dt/view/parameter.hpp"
#include "pxr/base/gf/vec2i.h"
#include "pxr/usd/sdf/path.h"
#include "pxr/usd/usd/primRange.h"
#include "pxr/usdImaging/usdImagingGL/engine.h"
#include "pxr/usdImaging/usdImagingGL/renderParams.h"
#include <string>
#include <vector>

namespace dt
{
  namespace view
  {
    class Render final : Parameter, public Controller
    {
    public:
      const std::string name;

      Render(const std::string &id);
      ~Render() = default;
      Render(Render &&);
      Render(const Render &) = delete;
      Render &operator=(Render &&) = delete;
      Render &operator=(const Render &) = delete;
      unsigned operator()(pxr::UsdStageRefPtr &stage);
      void SetToFreeCamera(pxr::UsdStageRefPtr &stage);
      void Draw();
      unsigned GetTexture();

      int GetWidth() const noexcept { return this->size[0]; }
      int GetHeight() const noexcept { return this->size[1]; }

      static void CachePaths(pxr::UsdStageRefPtr &stage)
      {
        std::vector<pxr::SdfPath> cache;
        cache.reserve(Render::paths.size());

        for (const pxr::UsdPrim &prim : stage->Traverse())
        {
          if (prim.IsA<pxr::UsdGeomCamera>())
          {
            cache.emplace_back(prim.GetPath());
          }
        }

        Render::paths = cache;
      }

    private:
      int pathIndex = 0;
      bool freeCamera = true;
      pxr::GfVec2i size = {1280, 720};
      pxr::UsdImagingGLEngine engine;

      void InitializeEngine();

      static inline std::vector<pxr::SdfPath> paths;

      static const char *PathGetter(void *user_data, int idx)
      {
        const pxr::SdfPath *paths = (pxr::SdfPath *)(user_data);
        return paths[idx].GetText();
      }
    };
  }
}