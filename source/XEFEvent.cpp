#include "XEFEvent.hpp"

XEFEvent::XEFEvent(
    XEFStream eventStream,
    int32_t frameIndex,
    int64_t relativeTimeTicks,
    std::shared_ptr<uint8_t[]> tagData,
    std::shared_ptr<uint8_t[]> eventData,
    uint32_t unknown)
    : eventStream(eventStream),
      frameIndex(frameIndex),
      relativeTimeTicks(relativeTimeTicks),
      tagData(tagData),
      eventData(eventData),
      unknown(unknown)
{
    isNull = false;
}

XEFEvent::XEFEvent() : isNull(true)
{
}