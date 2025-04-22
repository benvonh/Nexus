#pragma once
#include "dt/scene/manager.hpp"
#include "pxr/base/gf/camera.h"
#include "pxr/base/gf/frustum.h"

namespace dt
{
  namespace view
  {
    class Camera
    {
    public:
      float speed = 10.f;
      float sensitivity = 10.f;

      Camera() : camera(/*const GfMatrix4d &transform=*/pxr::GfMatrix4d(1),
                        /*Projection projection=*/pxr::GfCamera::Projection::Perspective,
                        /*float horizontalAperture=*/pxr::GfCamera::DEFAULT_HORIZONTAL_APERTURE,
                        /*float verticalAperture=*/pxr::GfCamera::DEFAULT_VERTICAL_APERTURE,
                        /*float horizontalApertureOffset=*/0.f,
                        /*float verticalApertureOffset=*/0.f,
                        /*float focalLength=*/18.f,
                        /*const GfRange1f &clippingRange=*/pxr::GfRange1f(1, 1000000),
                        /*const std::vector< GfVec4f > &clippingPlanes=*/std::vector<pxr::GfVec4f>(),
                        /*float fStop=*/8.f,
                        /*float focusDistance=*/0.f)
      {
        auto t = this->camera.GetTransform();
        t.SetTranslateOnly(pxr::GfVec3d(0, 0, 10));
        this->camera.SetTransform(t);
      }

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
      void Move(const float dt)
      {
        pxr::GfVec3d translate(0);
        pxr::GfMatrix4d transform = this->camera.GetTransform();
        const pxr::GfVec3d translation = transform.ExtractTranslation();
        const pxr::GfMatrix3d rotation = transform.ExtractRotationMatrix();
        const double step = this->speed * dt;

        if constexpr (D == Direction::FORWARD)
        {
          translate = rotation.GetColumn(2) * -step;
        }
        if constexpr (D == Direction::BACKWARD)
        {
          translate = rotation.GetColumn(2) * step;
        }
        if constexpr (D == Direction::LEFT)
        {
          translate = rotation.GetColumn(0) * -step;
        }
        if constexpr (D == Direction::RIGHT)
        {
          translate = rotation.GetColumn(0) * step;
        }
        if constexpr (D == Direction::UP)
        {
          translate[1] = step;
        }
        if constexpr (D == Direction::DOWN)
        {
          translate[1] = -step;
        }

        transform.SetTranslateOnly(translation + translate);
        this->camera.SetTransform(transform);
      }

      void Look(const float dx, const float dy, const float dt)
      {
        // float x, y;
        // this->rotateX.Get(&x, before);
        // this->rotateY.Get(&y, before);

        // const float q = -this->sensitivity * dt;
        // x += dy * q;
        // y += dx * q;

        // x = std::clamp(x, -90.f, 90.f);

        // this->rotateX.Set(x, now);
        // this->rotateY.Set(y, now);
      }

      inline pxr::GfFrustum GetFrustum() const
      {
        return this->camera.GetFrustum();
      }

    private:
      pxr::GfCamera camera;
    };
  }
}