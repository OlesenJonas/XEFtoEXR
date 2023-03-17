#pragma once

#include <memory>

#include "XEFStream.hpp"

struct XEFEvent
{
    bool isNull = false;

    XEFEvent();
    XEFEvent(
        std::shared_ptr<XEFStream> eventStream,
        int32_t frameIndex,
        int64_t relativeTimeTicks,
        int eventDataSize,
        std::shared_ptr<uint8_t[]> tagData,
        std::shared_ptr<uint8_t[]> eventData,
        uint32_t unknown);

    std::shared_ptr<XEFStream> eventStream;
    int32_t eventIndex;
    int32_t frameIndex;
    int64_t relativeTimeTicks;
    std::shared_ptr<uint8_t[]> tagData;
    int32_t eventDataSize;
    std::shared_ptr<uint8_t[]> rawEventData;
    uint32_t unknown;

    GUID getEventStreamDataTypeID() const;
};