#pragma once

#include "dt/api/api.h"

namespace dt
{
    struct DT_API IPanel
    {
        virtual ~IPanel() = default;

        virtual void OnRender() = 0;
    };
}