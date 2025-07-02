#pragma once
#include "dt/view/parameter.hpp"
#include "dt/view/controller.hpp"
#include "dt/view/filedialog.hpp"

#include "pxr/base/gf/vec2i.h"
#include "pxr/usd/sdf/path.h"
#include "pxr/usd/usd/primRange.h"
#include "pxr/usdImaging/usdImagingGL/engine.h"

#include <optional>
#include <string>
#include <vector>

namespace dt
{
  namespace view
  {
    class Render : public Controller, Parameter
    {
      struct HandleFile : FileDialog::HandleFile
      {
        HandleFile(size_t size) : pixels(size) {}
        void operator()(const std::string &path) override;
        std::vector<unsigned char> pixels;
        int height = 0;
        int width = 0;
      };

    public:
      Render() { Reset(); }
      ~Render() = default;

      Render(Render &&) = delete;
      Render(const Render &) = delete;
      Render &operator=(Render &&) = delete;
      Render &operator=(const Render &) = delete;

      unsigned operator()();
      void SetToFreeCamera();
      void Capture();
      void Draw();
      void Reset();

    private:
      unsigned _Get_Texture();

      void _Update_Camera_Paths(pxr::default_stage &stage)
      {
        std::vector<pxr::SdfPath> paths;
        paths.reserve(_sdfPaths.size());

        for (const pxr::UsdPrim &prim : stage->Traverse())
        {
          if (prim.IsA<pxr::UsdGeomCamera>())
          {
            paths.emplace_back(prim.GetPath());
          }
        }
        _paths = std::move(paths);
      }

      static const char *path_getter(void *user_data, int idx)
      {
        const auto *paths = (pxr::SdfPath *)(user_data);
        return paths[idx].GetText();
      }

      int _sdfPathIndex = 0;
      bool _freeCamera = true;
      pxr::GfVec2i _size = {1280, 720};
      std::vector<pxr::SdfPath> _sdfPaths;
      std::optional<pxr::UsdImagingGLEngine> _engine;
    };
  }
}