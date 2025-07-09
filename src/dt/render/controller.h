#pragma once

#include "pxr/base/gf/camera.h"

namespace dt
{
    class Controller
    {
    public:
        Controller();

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
        void Move(const float dt);

        void Look(const float dx, const float dy, const float dt);

        void TransformFrom(const pxr::GfCamera &camera);

        double yaw = 0.0;
        double pitch = 90.0;

        pxr::GfCamera camera;

        static inline float Speed = 5.f;
        static inline float Sense = 10.f;

    };
}