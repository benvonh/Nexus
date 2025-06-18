#pragma once
#include "dt/scene/manager.hpp"
#include "dt/view/render.hpp"
#include <vector>

namespace dt
{
  namespace view
  {
    class RenderGroup
    {
    public:
      RenderGroup() = default;
      ~RenderGroup() = default;
      RenderGroup(RenderGroup &&) = delete;
      RenderGroup(const RenderGroup &) = delete;
      RenderGroup &operator=(RenderGroup &&) = delete;
      RenderGroup &operator=(const RenderGroup &) = delete;

      void SetToDefault()
      {
        _renders.clear();
        _renders.emplace_back("Viewport");
      }

      void AddRender()
      {
        _renders.emplace_back(std::format("Viewport (#{})", _renders.size()));
      }

      void Draw()
      {
        scene::StagePermit permit = scene::Manager::GetStagePermit();
        Render::CachePaths(permit.stage);

        for (int i = 0; i < _renders.size(); ++i)
        {
          Render &render = _renders[i];
          ImGui::Begin(render.name.c_str(), nullptr, ImGuiWindowFlags_MenuBar);

          render.Draw();

          if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
          {
            if (i != permit.active)
            {
              permit.active = i;
              render.SetToFreeCamera(permit.stage);
              SDL_SetWindowRelativeMouseMode(permit.window, true);
              ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse;
            }
          }
          ImGui::End();
        }
      }

    private:
      void _Cache_Paths(pxr::UsdStageRefPtr &stage)
      {
        std::vector<pxr::SdfPath> cache;
        cache.reserve(_paths.size());

        for (const pxr::UsdPrim &prim : stage->Traverse())
        {
          if (prim.IsA<pxr::UsdGeomCamera>())
          {
            cache.emplace_back(prim.GetPath());
          }
        }
        _paths = std::move(cache);
      }

      Render *_active = nullptr;
      std::vector<Render> _renders;
      std::vector<pxr::SdfPath> _paths;
    };
  }
}