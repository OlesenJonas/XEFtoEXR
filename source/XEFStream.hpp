#pragma once

#include <cstdint>
#include <memory>
#include <string>

#include "Constants.hpp"
#include "GUID.hpp"

struct XEFStream
{
    bool isNull = false;

    XEFStream();

    XEFStream(
        int16_t streamIndex,
        int16_t streamFlags,
        int32_t tagSize,
        std::wstring dataTypeName,
        GUID dataTypeID,
        GUID semanticID);

    int32_t eventsCount = 0;
    int16_t streamIndex;
    int16_t streamFlags;
    int32_t tagSize = 0;
    std::wstring dataTypeName;
    GUID dataTypeID;
    GUID semanticID;

    [[nodiscard]] inline bool isCompressed() const
    {
        return (streamFlags & DataConstants::FLAG_COMPRESSED) == DataConstants::FLAG_COMPRESSED;
    }
};

// Dont really like this being a shared ptr and all that but for now whatever
inline std::shared_ptr<XEFStream> nullStream = std::make_shared<XEFStream>();