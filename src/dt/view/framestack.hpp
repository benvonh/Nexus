#pragma once
#include "dt/scene/manager.hpp"
#include "dt/view/render.hpp"

namespace dt
{
  namespace view
  {
    template <unsigned long long MAX = 4>
    class FrameStack
    {
    public:
      FrameStack() = default;
      ~FrameStack() = default;

      FrameStack(FrameStack &&) = delete;
      FrameStack(const FrameStack &) = delete;
      FrameStack &operator=(FrameStack &&) = delete;
      FrameStack &operator=(const FrameStack &) = delete;

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

      Render _renders[MAX];
      Render *_active = nullptr;
      std::vector<pxr::SdfPath> _paths;
    };
  }
}