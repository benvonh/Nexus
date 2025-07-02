#include "controller.h"
#include "imgui.h"

/*============================================================================*/
dt::Controller::Controller()
    : _Camera(/*const GfMatrix4d &transform*/ pxr::GfMatrix4d(1),
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

/*============================================================================*/
void dt::Controller::look(const float dx, const float dy, const float dt)
{
    const double q = _Sense * -dt;

    _Yaw += dx * q;
    _Pitch += dy * q;
    _Pitch = std::clamp(_Pitch, 0.0, 180.0);

    pxr::GfRotation rotationZ(pxr::GfVec3d(0, 0, 1), _Yaw);
    pxr::GfRotation rotationX(pxr::GfVec3d(1, 0, 0), _Pitch);

    pxr::GfMatrix3d rotation = rotationX * rotationZ;
    pxr::GfMatrix4d transform = _Camera.GetTransform();

    transform.SetRotateOnly(rotation);
    _Camera.SetTransform(transform);
}

/*============================================================================*/
void dt::Controller::transform_from(const pxr::GfCamera &camera)
{
    pxr::GfMatrix4d transform = camera.GetTransform();
    pxr::GfVec3d rpy = transform.DecomposeRotation(pxr::GfVec3d(0, 0, 1),
                                                   pxr::GfVec3d(0, 1, 0),
                                                   pxr::GfVec3d(1, 0, 0));
    _Yaw = rpy[0];
    _Pitch = rpy[2];
    _Camera.SetTransform(transform);
}

/*============================================================================*/
void dt::Controller::draw()
{
    ImGui::Begin("Controller");
    ImGui::InputFloat("Speed", &_Speed, 0.1f, 20.f, "%.0f");
    ImGui::InputFloat("Sensitivity", &_Sense, 0.1f, 20.f, "%.0f");
    ImGui::End();
}