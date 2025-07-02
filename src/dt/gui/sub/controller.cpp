#include "controller.h"
#include "imgui.h"
#include <algorithm>

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
    this->look(0, 0, 0); // Ensure camera is initialized to default angles
    __Camera = _Camera;  // Yes, we re-initialize the static camera every time
}

/*============================================================================*/
template <dt::Controller::Direction D>
void dt::Controller::move(const float dt)
{
    pxr::GfVec3d translate(0);
    pxr::GfMatrix4d transform = _Camera.GetTransform();
    const pxr::GfVec3d translation = transform.ExtractTranslation();
    const pxr::GfMatrix3d rotation = transform.ExtractRotationMatrix();
    const double step = __Speed * dt;

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
    _Camera.SetTransform(transform);
}

template void dt::Controller::move<dt::Controller::Direction::FORWARD>(const float dt);
template void dt::Controller::move<dt::Controller::Direction::BACKWARD>(const float dt);
template void dt::Controller::move<dt::Controller::Direction::LEFT>(const float dt);
template void dt::Controller::move<dt::Controller::Direction::RIGHT>(const float dt);
template void dt::Controller::move<dt::Controller::Direction::UP>(const float dt);
template void dt::Controller::move<dt::Controller::Direction::DOWN>(const float dt);

/*============================================================================*/
void dt::Controller::look(const float dx, const float dy, const float dt)
{
    const double q = __Sense * -dt;

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
void dt::Controller::Draw()
{
    ImGui::Begin("Controller");
    ImGui::Text("Yaw: %.1f", __Yaw);
    ImGui::Text("Pitch: %.1f", __Pitch);
    ImGui::InputFloat("Speed", &__Speed, 1.f, 20.f, "%.0f");
    ImGui::InputFloat("Sensitivity", &__Sense, 1.f, 20.f, "%.0f");
    ImGui::End();
}

/*============================================================================*/
void dt::Controller::draw()
{
    if (ImGui::BeginMenu("Camera Controller"))
    {
        if (ImGui::Button("Set Camera from Global"))
        {
            _Yaw = __Yaw;
            _Pitch = __Pitch;
            _Camera = __Camera;
        }

        // TODO: Better phrasing
        if (ImGui::Button("Set Global Camera to This"))
        {
            __Yaw = _Yaw;
            __Pitch = _Pitch;
            __Camera = _Camera;
        }
        ImGui::EndMenu();
    }
    ImGui::Text("Yaw: %.1f | Pitch: %.1f", _Yaw, _Pitch);
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