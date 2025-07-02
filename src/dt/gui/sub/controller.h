#pragma once
#include "pxr/base/gf/camera.h"
#include "pxr/base/gf/frustum.h"
#include "pxr/base/gf/matrix3d.h"
#include "pxr/base/gf/matrix4d.h"
#include "pxr/base/gf/range1f.h"
#include "pxr/base/gf/rotation.h"
#include "pxr/base/gf/vec3d.h"
#include "pxr/base/gf/vec4f.h"

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
        void move(const float dt);

        void look(const float dx, const float dy, const float dt);

        static void Draw();

    protected:
        void draw();

        auto get_frustum() const { return _Camera.GetFrustum(); }

        void transform_from(const pxr::GfCamera &camera);

    private:
        double _Yaw = 0.0;
        double _Pitch = 90.0;
        pxr::GfCamera _Camera;

        static inline float __Speed = 10.f;
        static inline float __Sense = 10.f;
        static inline double __Yaw = 0.0;
        static inline double __Pitch = 90.0;
        static inline pxr::GfCamera __Camera;
    };
}