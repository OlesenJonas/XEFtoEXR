#include "XEFStream.hpp"

XEFStream::XEFStream() : isNull(true)
{
}

XEFStream::XEFStream(
    int16_t streamIndex,
    int16_t streamFlags,
    int32_t tagSize,
    std::wstring dataTypeName,
    GUID dataTypeID,
    GUID semanticID)
    : eventsCount(0),
      streamIndex(streamIndex),
      streamFlags(streamFlags),
      dataTypeName(dataTypeName),
      tagSize(tagSize),
      dataTypeID(dataTypeID),
      semanticID(semanticID)
{
}