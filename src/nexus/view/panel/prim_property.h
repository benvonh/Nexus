#pragma once

#include "pxr/usd/usd/prim.h"

namespace Nexus
{
    class PrimProperty final
    {
    public:
        PrimProperty();

        void draw();

    private:
        pxr::UsdPrim m_ContextPrim;
    };
}