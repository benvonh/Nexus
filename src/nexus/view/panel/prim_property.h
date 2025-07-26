#pragma once

#include "nexus/logging.h"

#include "pxr/usd/usd/prim.h"

namespace Nexus
{
    class PrimProperty : Logger<"Prim Property">
    {
    public:
        PrimProperty();

        void draw();

    private:
        pxr::UsdPrim m_ContextPrim;
    };
}