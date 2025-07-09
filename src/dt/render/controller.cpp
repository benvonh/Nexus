#include "controller.h"

#include "pxr/base/gf/vec3d.h"
#include "pxr/base/gf/vec4f.h"
#include "pxr/base/gf/range1f.h"
#include "pxr/base/gf/matrix3d.h"
#include "pxr/base/gf/matrix4d.h"
#include "pxr/base/gf/rotation.h"

#include <vector>
#include <algorithm>

dt::Controller::Controller()
    : camera(/*const GfMatrix4d &transform*/ pxr::GfMatrix4d(1),
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
    this->Look(0, 0, 0);
}

template void dt::Controller::Move<dt::Controller::Direction::FORWARD>(const float);
template void dt::Controller::Move<dt::Controller::Direction::BACKWARD>(const float);
template void dt::Controller::Move<dt::Controller::Direction::LEFT>(const float);
template void dt::Controller::Move<dt::Controller::Direction::RIGHT>(const float);
template void dt::Controller::Move<dt::Controller::Direction::UP>(const float);
template void dt::Controller::Move<dt::Controller::Direction::DOWN>(const float);

template <dt::Controller::Direction D>
void dt::Controller::Move(const float dt)
{
    pxr::GfVec3d translate(0);
    pxr::GfMatrix4d transform = this->camera.GetTransform();
    const pxr::GfVec3d translation = transform.ExtractTranslation();
    const pxr::GfMatrix3d rotation = transform.ExtractRotationMatrix();
    const double step = this->Speed * dt;

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

void dt::Controller::Look(const float dx, const float dy, const float dt)
{
    const double q = Sense * -dt;

    this->yaw += dx * q;
    this->pitch += dy * q;

    this->yaw = this->yaw > 180.0 ? this->yaw - 360.0 : this->yaw;
    this->yaw = this->yaw < -180.0 ? this->yaw + 360.0 : this->yaw;
    this->pitch = std::clamp(this->pitch, 0.0, 180.0);

    pxr::GfRotation rotationZ(pxr::GfVec3d(0, 0, 1), this->yaw);
    pxr::GfRotation rotationX(pxr::GfVec3d(1, 0, 0), this->pitch);

    pxr::GfMatrix3d rotation = rotationX * rotationZ;
    pxr::GfMatrix4d transform = this->camera.GetTransform();

    transform.SetRotateOnly(rotation);
    this->camera.SetTransform(transform);
}

void dt::Controller::TransformFrom(const pxr::GfCamera &camera)
{
    pxr::GfMatrix4d transform = camera.GetTransform();
    pxr::GfVec3d rpy = transform.DecomposeRotation(pxr::GfVec3d(0, 0, 1),
                                                   pxr::GfVec3d(0, 1, 0),
                                                   pxr::GfVec3d(1, 0, 0));
    this->yaw = rpy[0];
    this->pitch = rpy[2];
    this->camera.SetTransform(transform);
}