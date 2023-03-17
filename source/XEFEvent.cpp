#include "XEFEvent.hpp"

XEFEvent::XEFEvent() : isNull(true)
{
}

XEFEvent::XEFEvent(
    std::shared_ptr<XEFStream> eventStream,
    int32_t frameIndex,
    int64_t relativeTimeTicks,
    int eventDataSize,
    std::shared_ptr<uint8_t[]> tagData,
    std::shared_ptr<uint8_t[]> eventData,
    uint32_t unknown)
    : eventStream(eventStream),
      frameIndex(frameIndex),
      relativeTimeTicks(relativeTimeTicks),
      tagData(tagData),
      eventDataSize(eventDataSize),
      rawEventData(eventData),
      unknown(unknown)
{
    isNull = false;

    if(!eventStream->isNull)
    {
        eventIndex = eventStream->eventsCount;
        eventStream->eventsCount++;
    }
}

GUID XEFEvent::getEventStreamDataTypeID() const
{
    if(eventStream->isNull)
        return StreamDataTypeIds::Null;
    return eventStream->dataTypeID;
}