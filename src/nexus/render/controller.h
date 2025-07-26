#pragma once

#include "pxr/base/gf/camera.h"

namespace Nexus
{
    struct Controller
    {
        Controller();

        enum class Direction : char
        {
            FORWARD,
            BACKWARD,
            LEFT,
            RIGHT,
            UP,
            DOWN
        };

        template <Direction D>
        void move(const float dt);

        void look(const float dx, const float dy, const float dt);

        void transform_from(const pxr::GfCamera &camera);

        double Yaw = 0.0;
        double Pitch = 90.0;

        pxr::GfCamera Camera;

        static inline float SPEED = 5.f;
        static inline float SENSITIVITY = 10.f;
    };
}