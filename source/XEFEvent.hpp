#pragma once

#include <memory>

#include "XEFStream.hpp"

struct XEFEvent
{
    bool isNull = false;

    XEFEvent(
        XEFStream eventStream,
        int32_t frameIndex,
        int64_t relativeTimeTicks,
        std::shared_ptr<uint8_t[]> tagData,
        std::shared_ptr<uint8_t[]> eventData,
        uint32_t unknown);
    XEFEvent();

    XEFStream eventStream;
    int32_t frameIndex;
    int64_t relativeTimeTicks;
    std::shared_ptr<uint8_t[]> tagData;
    std::shared_ptr<uint8_t[]> eventData;
    uint32_t unknown;
};