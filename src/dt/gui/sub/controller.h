#pragma once
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
    class Controller
    {
    public:
        Controller();

        void draw();

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
        void move(const float dt)
        {
            pxr::GfVec3d translate(0);
            pxr::GfMatrix4d transform = _Camera.GetTransform();
            const pxr::GfVec3d translation = transform.ExtractTranslation();
            const pxr::GfMatrix3d rotation = transform.ExtractRotationMatrix();
            const double step = _Speed * dt;

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

        void look(const float dx, const float dy, const float dt);

        void transform_from(const pxr::GfCamera &camera);

        pxr::GfFrustum get_frustum() const { return _Camera.GetFrustum(); }

    private:
        float _Speed = 10.f;
        float _Sense = 10.f;
        double _Yaw = 0.0;
        double _Pitch = 90.0;
        pxr::GfCamera _Camera;
    };
}