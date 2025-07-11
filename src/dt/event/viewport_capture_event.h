#pragma once

#include "event.h"

namespace dt
{
    struct ViewportCaptureEvent : public Event
    {
        ViewportCaptureEvent(bool capture) : Capture(capture) {}
        const bool Capture;
    };
}