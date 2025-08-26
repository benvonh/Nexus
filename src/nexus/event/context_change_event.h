#pragma once

#include "nexus/event/event.h"

#include "pxr/usd/usd/prim.h"

namespace Nexus
{
    struct ContextChangeEvent : Event
    {
        ContextChangeEvent(const pxr::UsdPrim &prim) : Prim(prim) {}
        pxr::UsdPrim Prim;
    };
}