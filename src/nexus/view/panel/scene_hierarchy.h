#pragma once

#include "pxr/usd/usd/primRange.h"

namespace Nexus
{
    class SceneHierarchy
    {
    public:
        SceneHierarchy();

        void draw();

    private:
        std::size_t m_ContextHash = 0;

        pxr::UsdPrimRange m_Range;
    };
}