#pragma once

#include "nexus/event/event.h"

namespace Nexus
{
    struct MouseEvent : Event
    {
        MouseEvent(int x, int y, float dt) : X(x), Y(y), Tick(dt) {}
        const float X;
        const float Y;
        const float Tick;
    };
}