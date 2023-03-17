#include "DepthFrame.hpp"

#include <cassert>
#include <stdint.h>

#include "XEFEvent.hpp"

bool DepthFrame::replacePixelsWithNewDepthEvent(const XEFEvent& event)
{
    assert(event.getEventStreamDataTypeID() == StreamDataTypeIds::Depth);
    constexpr auto amountOfPixels = 512 * 424;
    assert(amountOfPixels * sizeof(uint16_t) == event.eventDataSize);
    // TODO: not sure if signed or unsigned, but I guess its unsigned
    auto const* ushortPixels = reinterpret_cast<uint16_t*>(event.rawEventData.get());

    auto* targetFloatPixels = reinterpret_cast<float*>(floatPixels.data);
    for(auto i = 0; i < amountOfPixels; i++)
    {
        targetFloatPixels[i] = ushortPixels[i];
    }

    return true;
}