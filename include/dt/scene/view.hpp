#pragma once
#include "pxr/usd/usdGeom/camera.h"
#include "pxr/usd/usdGeom/xformOp.h"

namespace dt
{
  namespace scene
  {
    class View final
    {
      // Only a friend class may construct this object.
      // Once instantiated, the object cannot be moved or copied.
      friend class Manager;

    public:
      View(View &&) = delete;
      View(const View &) = delete;
      View &operator=(View &&) = delete;
      View &operator=(const View &) = delete;

      enum class Direction
      {
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT,
        UP,
        DOWN
      };

      template <Direction D>
      void Move(const double dt);
      void Look(const float dx, const float dy, const float dt);

    private:
      View() = default;
      ~View() = default;

      float speed = 10.f;
      float sensitivity = 10.f;
      pxr::UsdGeomCamera camera;
      pxr::UsdGeomXformOp rotateX;
      pxr::UsdGeomXformOp rotateY;
      pxr::UsdGeomXformOp translate;
    };
  }
}