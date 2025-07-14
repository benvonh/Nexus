#pragma once

#include "dt/event/client.h"

#include "pxr/usd/usd/prim.h"

namespace dt
{
    class Property : Client
    {
    public:
        Property();

        void draw();

    private:
        pxr::UsdPrim _Context_prim;
    };
}