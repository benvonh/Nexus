#pragma once
#include "pxr/base/gf/camera.h"
#include "pxr/base/gf/matrix3d.h"
#include "pxr/base/gf/matrix4d.h"
#include "pxr/base/gf/rotation.h"
#include "pxr/base/gf/vec3d.h"
#include <algorithm>

namespace dt
{
namespace view
{
class Camera
{
public:
  int index = 0;
  bool free = true;
  float speed = 10.0;
  float sensitivity = 10.0;
  pxr::GfCamera data;

  Camera() : data(
    /*const GfMatrix4d &transform*/ pxr::GfMatrix4d(1),
    /*Projection projection*/ pxr::GfCamera::Projection::Perspective,
    /*float horizontalAperture*/ pxr::GfCamera::DEFAULT_HORIZONTAL_APERTURE,
    /*float verticalAperture*/ pxr::GfCamera::DEFAULT_VERTICAL_APERTURE,
    /*float horizontalApertureOffset*/ 0.f,
    /*float verticalApertureOffset*/ 0.f,
    /*float focalLength*/ 18.f,
    /*const GfRange1f &clippingRange*/ pxr::GfRange1f(1, 1000000),
    /*const std::vector<GfVec4f> &clippingPlanes*/ std::vector<pxr::GfVec4f>(),
    /*float fStop*/ 8.f,
    /*float focusDistance*/ 0.f
  ) {}
  ~Camera() = default;
  Camera(Camera &&) = delete;
  Camera(const Camera &) = delete;
  Camera &operator=(Camera &&) = delete;
  Camera &operator=(const Camera &) = delete;

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
    pxr::GfMatrix4d transform = this->data.GetTransform();
    const pxr::GfVec3d translation = transform.ExtractTranslation();
    const pxr::GfMatrix3d rotation = transform.ExtractRotationMatrix();
    const double step = this->speed * dt;

    if constexpr (D == Direction::FORWARD) {
      translate = rotation.GetRow(2) * -step;
    }
    if constexpr (D == Direction::BACKWARD) {
      translate = rotation.GetRow(2) * step;
    }
    if constexpr (D == Direction::LEFT) {
      translate = rotation.GetRow(0) * -step;
    }
    if constexpr (D == Direction::RIGHT) {
      translate = rotation.GetRow(0) * step;
    }
    if constexpr (D == Direction::UP) {
      translate[1] = step;
    }
    if constexpr (D == Direction::DOWN) {
      translate[1] = -step;
    }

    transform.SetTranslateOnly(translation + translate);
    this->data.SetTransform(transform);
  }

  void Look(const float dx, const float dy, const float dt)
  {
    const double q = -this->sensitivity * dt;
    
    this->yaw += dx * q;
    this->pitch += dy * q;
    this->pitch = std::clamp(this->pitch, -90.0, 90.0);

    pxr::GfRotation rotationY(pxr::GfVec3d(0, 1, 0), this->yaw);
    pxr::GfRotation rotationX(pxr::GfVec3d(1, 0, 0), this->pitch);

    pxr::GfMatrix3d rotation = rotationX * rotationY;
    pxr::GfMatrix4d transform = this->data.GetTransform();

    transform.SetRotateOnly(rotation);
    this->data.SetTransform(transform);
  }

private:
  double yaw = 0.0;
  double pitch = 0.0;
};
} // namespace view
} // namespace dt