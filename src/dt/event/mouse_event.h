#pragma once

#include "dt/event/event.h"

namespace dt
{
    struct MouseEvent : Event
    {
        MouseEvent(int x, int y, float dt) : X(x), Y(y), Tick(dt) {}
        const float X;
        const float Y;
        const float Tick;
    };
}