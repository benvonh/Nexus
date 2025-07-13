#pragma once

#include "dt/event/client.h"

#include "pxr/usd/sdf/path.h"
#include "pxr/usd/usd/primRange.h"

namespace dt
{
    class SceneHierarchy : Client
    {
    public:
        SceneHierarchy();

        void draw();

    private:
        size_t M_ContextHash = 0;

        pxr::UsdPrimRange M_Range;
    };
}