#pragma once
#include "imgui.h"
#include "pxr/base/gf/camera.h"
#include "pxr/base/gf/frustum.h"
#include "pxr/base/gf/matrix3d.h"
#include "pxr/base/gf/matrix4d.h"
#include "pxr/base/gf/range1f.h"
#include "pxr/base/gf/rotation.h"
#include "pxr/base/gf/vec3d.h"
#include "pxr/base/gf/vec4f.h"
#include <algorithm>

namespace dt
{
  namespace view
  {
    class Controller
    {
    public:
      Controller()
          : camera(/*const GfMatrix4d &transform*/ pxr::GfMatrix4d(1),
                   /*Projection projection*/ pxr::GfCamera::Projection::Perspective,
                   /*float horizontalAperture*/ pxr::GfCamera::DEFAULT_HORIZONTAL_APERTURE,
                   /*float verticalAperture*/ pxr::GfCamera::DEFAULT_VERTICAL_APERTURE,
                   /*float horizontalApertureOffset*/ 0.f,
                   /*float verticalApertureOffset*/ 0.f,
                   /*float focalLength*/ 18.f,
                   /*const GfRange1f &clippingRange*/ pxr::GfRange1f(1, 1000000),
                   /*const std::vector<GfVec4f> &clippingPlanes*/ std::vector<pxr::GfVec4f>(),
                   /*float fStop*/ 8.f,
                   /*float focusDistance*/ 0.f)
      {
        this->Look(0, 0, 0);
      }

      Controller(Controller &&other) noexcept
      {
        this->yaw = other.yaw;
        this->pitch = other.pitch;
        this->camera = std::move(other.camera);
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
          translate = rotation.GetRow(2) * -step;
        }
        if constexpr (D == Direction::BACKWARD)
        {
          translate = rotation.GetRow(2) * step;
        }
        if constexpr (D == Direction::LEFT)
        {
          translate = rotation.GetRow(0) * -step;
        }
        if constexpr (D == Direction::RIGHT)
        {
          translate = rotation.GetRow(0) * step;
        }
        if constexpr (D == Direction::UP)
        {
          translate[2] = step;
        }
        if constexpr (D == Direction::DOWN)
        {
          translate[2] = -step;
        }
        transform.SetTranslateOnly(translation + translate);
        this->camera.SetTransform(transform);
      }

      void Look(const float dx, const float dy, const float dt)
      {
        const double q = -this->sensitivity * dt;

        this->yaw += dx * q;
        this->pitch += dy * q;
        this->pitch = std::clamp(this->pitch, 0.0, 180.0);

        pxr::GfRotation rotationZ(pxr::GfVec3d(0, 0, 1), this->yaw);
        pxr::GfRotation rotationX(pxr::GfVec3d(1, 0, 0), this->pitch);

        pxr::GfMatrix3d rotation = rotationX * rotationZ;
        pxr::GfMatrix4d transform = this->camera.GetTransform();

        transform.SetRotateOnly(rotation);
        this->camera.SetTransform(transform);
      }

      void TransformFrom(const pxr::GfCamera &camera)
      {
        pxr::GfMatrix4d transform = camera.GetTransform();
        pxr::GfVec3d rpy = transform.DecomposeRotation(pxr::GfVec3d(0, 0, 1),
                                                       pxr::GfVec3d(0, 1, 0),
                                                       pxr::GfVec3d(1, 0, 0));
        this->yaw = rpy[0];
        this->pitch = rpy[2];
        this->camera.SetTransform(transform);
      }

      pxr::GfFrustum Frustum() const noexcept
      {
        return this->camera.GetFrustum();
      }

      static void Draw()
      {
        ImGui::InputFloat("Speed", &Controller::speed, 0.1f, 20.f, "%.0f");
        ImGui::InputFloat("Sensitivity", &Controller::sensitivity, 0.1f, 20.f, "%.0f");
      }

    private:
      double yaw = 0.0;
      double pitch = 90.0;
      pxr::GfCamera camera;

      static inline float speed = 10.f;
      static inline float sensitivity = 10.f;
    };
  }
}