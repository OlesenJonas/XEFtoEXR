#include "ColorFrame.hpp"

#include <cassert>

#include "XEFEvent.hpp"

bool ColorFrame::replacePixelsWithNewUncompressedColorEvent(const XEFEvent& event)
{
    assert(event.getEventStreamDataTypeID() == StreamDataTypeIds::UncompressedColor);

    // unpack yuyv packing + convert yuv to rgb
    //  todo: optimize!

    return true;
}