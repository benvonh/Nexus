#include "controller.h"

#include "pxr/base/gf/matrix3d.h"
#include "pxr/base/gf/matrix4d.h"
#include "pxr/base/gf/range1f.h"
#include "pxr/base/gf/rotation.h"
#include "pxr/base/gf/vec3d.h"
#include "pxr/base/gf/vec4f.h"

#include <algorithm>
#include <vector>

dt::Controller::Controller()
    : Camera(/*const GfMatrix4d &transform*/ pxr::GfMatrix4d(1),
             /*Projection projection*/ pxr::GfCamera::Projection::Perspective,
             /*float horizontalAperture*/ pxr::GfCamera::DEFAULT_HORIZONTAL_APERTURE * 0.01,
             /*float verticalAperture*/ pxr::GfCamera::DEFAULT_VERTICAL_APERTURE * 0.01,
             /*float horizontalApertureOffset*/ 0.f,
             /*float verticalApertureOffset*/ 0.f,
             /*float focalLength*/ 18.f * 0.01,
             /*const GfRange1f &clippingRange*/ pxr::GfRange1f(1 * 0.01, 1000000 * 0.01),
             /*const std::vector<GfVec4f> &clippingPlanes*/ std::vector<pxr::GfVec4f>(),
             /*float fStop*/ 8.f * 0.01,
             /*float focusDistance*/ 0.f)
{
    this->look(0, 0, 0);
}

template void dt::Controller::move<dt::Controller::Direction::FORWARD>(const float);
template void dt::Controller::move<dt::Controller::Direction::BACKWARD>(const float);
template void dt::Controller::move<dt::Controller::Direction::LEFT>(const float);
template void dt::Controller::move<dt::Controller::Direction::RIGHT>(const float);
template void dt::Controller::move<dt::Controller::Direction::UP>(const float);
template void dt::Controller::move<dt::Controller::Direction::DOWN>(const float);

template <dt::Controller::Direction D>
void dt::Controller::move(const float dt)
{
    pxr::GfVec3d translate(0);
    pxr::GfMatrix4d transform = this->Camera.GetTransform();
    const pxr::GfVec3d translation = transform.ExtractTranslation();
    const pxr::GfMatrix3d rotation = transform.ExtractRotationMatrix();
    const double step = this->SPEED * dt;

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
    this->Camera.SetTransform(transform);
}

void dt::Controller::look(const float dx, const float dy, const float dt)
{
    const double q = this->SENSITIVITY * -dt;

    this->Yaw += dx * q;
    this->Pitch += dy * q;

    this->Yaw = this->Yaw > 180.0 ? this->Yaw - 360.0 : this->Yaw;
    this->Yaw = this->Yaw < -180.0 ? this->Yaw + 360.0 : this->Yaw;
    this->Pitch = std::clamp(this->Pitch, 0.0, 180.0);

    pxr::GfRotation rotationZ(pxr::GfVec3d(0, 0, 1), this->Yaw);
    pxr::GfRotation rotationX(pxr::GfVec3d(1, 0, 0), this->Pitch);

    pxr::GfMatrix3d rotation = rotationX * rotationZ;
    pxr::GfMatrix4d transform = this->Camera.GetTransform();

    transform.SetRotateOnly(rotation);
    this->Camera.SetTransform(transform);
}

void dt::Controller::transform_from(const pxr::GfCamera &camera)
{
    pxr::GfMatrix4d transform = camera.GetTransform();
    pxr::GfVec3d rpy = transform.DecomposeRotation(pxr::GfVec3d(0, 0, 1),
                                                   pxr::GfVec3d(0, 1, 0),
                                                   pxr::GfVec3d(1, 0, 0));
    this->Yaw = rpy[0];
    this->Pitch = rpy[2];
    this->Camera.SetTransform(transform);
}