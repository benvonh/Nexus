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

        static void Draw();

    protected:
        void __transform_from(const pxr::GfCamera &camera);

        double _Yaw = 0.0;
        double _Pitch = 90.0;

        pxr::GfCamera _Camera;

    private:
        static inline float __Speed = 5.f;
        static inline float __Sense = 10.f;
    };
}