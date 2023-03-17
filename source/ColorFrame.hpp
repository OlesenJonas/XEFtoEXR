#pragma once

#include <libfreenect2/frame_listener.hpp>

// only need forward declaration
struct XEFEvent;

struct YUYV422Packet
{
    uint8_t yuyv[4];
};
struct RGBA
{
    uint8_t rgba[4];
};

struct ColorFrame
{
    libfreenect2::Frame rgbPixels{1920, 1080, 4};

    bool replacePixelsWithNewUncompressedColorEvent(const XEFEvent& event);
};