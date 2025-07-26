#pragma once

#include "nexus/event/event.h"

#include "SDL3/SDL_scancode.h"

namespace Nexus
{
    struct KeyboardEvent : Event
    {
        KeyboardEvent(const bool *keys, float dt) : Keys(keys), Tick(dt) {}
        const bool *Keys;
        const float Tick;
    };
}