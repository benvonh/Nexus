#pragma once

#include "nexus/logging.h"

#include "pxr/usd/usd/prim.h"

namespace Nexus
{
    class Property : Logger<"Property Panel">
    {
    public:
        Property();

        void draw();

    private:
        pxr::UsdPrim m_ContextPrim;
    };
}