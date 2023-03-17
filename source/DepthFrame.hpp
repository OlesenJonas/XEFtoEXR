#pragma once

#include <libfreenect2/frame_listener.hpp>

struct XEFEvent;

struct DepthFrame
{
    libfreenect2::Frame floatPixels{512, 424, 4};

    bool replacePixelsWithNewDepthEvent(const XEFEvent& event);
};