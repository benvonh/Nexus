#pragma once

#include "event.h"

namespace Nexus
{
    struct ViewportCaptureEvent : public Event
    {
        ViewportCaptureEvent(bool capture) : Capture(capture) {}
        const bool Capture;
    };
}