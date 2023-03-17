#include "XEFReader.hpp"
#include "Constants.hpp"

#include <cassert>
#include <ios>
#include <memory>
#include <stdint.h>

XEFReader::XEFReader(std::string_view path) : path(path)
{
    file = std::ifstream{this->path, std::ios::in | std::ios::binary};
    assert(file.is_open());
    assert(file.tellg() == 0);

    // Read xef header
    // skip first 12 bytes
    file.seekg(12, std::ios_base::cur);
    int32_t numberOfStreams = 0;
    read<int32_t>(&numberOfStreams);
    streamCount = numberOfStreams - 1;
    assert(streamCount > 0);
    // skip the next 8 bytes
    file.seekg(8, std::ios_base::cur);

    eventStartAdress = file.tellg();
}

XEFReader::~XEFReader()
{
    file.close();
}

XEFReader::EventKey XEFReader::peekNextEventKey()
{
    const auto prev = file.tellg();
    EventKey eventKey;
    read<int16_t>(&eventKey.streamIndex);
    read<int16_t>(&eventKey.streamFlags);
    file.seekg(-2 * 2, std::ios_base::cur);
    assert(file.tellg() == prev);
    return eventKey;
}
XEFReader::EventKey XEFReader::readNextEventKey()
{
    EventKey eventKey;
    read<int16_t>(&eventKey.streamIndex);
    read<int16_t>(&eventKey.streamFlags);
    return eventKey;
}

XEFEvent XEFReader::getNextEvent()
{
    if(endOfStream)
    {
        return XEFEvent();
    }

    const EventKey eventKey = peekNextEventKey();

    if(eventKey.streamIndex == DataConstants::EVENT_UNKRECORD_INDEX)
    {
        // consume unknown event from stream and return next one
        readUnknownEvent();
        return getNextEvent();
    }

    if(eventKey.streamIndex == streamCount + 1)
    {
        endOfStream = true;
        return XEFEvent();
    }

    if(streams.find(eventKey.streamIndex) == streams.end())
    {
        streams[eventKey.streamIndex] = std::make_shared<XEFStream>(readStreamDescription());

        return getNextEvent();
    }

    return readDataEvent();
}

bool XEFReader::isValidStreamIndex(int16_t index) const
{
    // Is the stream a valid (non-property) stream, or is unknown or is signaling end of file
    return (index > 0 && index <= streamCount) || index == DataConstants::EVENT_UNKRECORD_INDEX ||
           index == streamCount + 1;
}

void XEFReader::readUnknownEvent()
{
    EventKey eventKey = readNextEventKey();
    assert(eventKey.streamIndex == DataConstants::EVENT_UNKRECORD_INDEX);

    file.seekg(sizeof(int32_t), std::ios_base::cur); // unknown id
    file.seekg(sizeof(int64_t), std::ios_base::cur); // Timestamp
    file.seekg(sizeof(int32_t), std::ios_base::cur); // Null
    file.seekg(sizeof(int32_t), std::ios_base::cur); // Null

    // Right now, we'll just heuristically skip through by 0x1000 at a time until we find a valid stream id
    // TODO Figure out better way to identify how long an unknown record is
    //      from: https://github.com/Isaac-W/KinectXEFTools.
    file.seekg(0x6000, std::ios_base::cur);
    eventKey = peekNextEventKey();

    if(!isValidStreamIndex(eventKey.streamIndex))
    {
        // Try next position (0x7000 total length)
        file.seekg(0x1000, std::ios_base::cur);
        eventKey = peekNextEventKey();

        if(!isValidStreamIndex(eventKey.streamIndex))
        {
            // Treat as long record (0xC000 total length)
            file.seekg(0x5000, std::ios_base::cur);

            eventKey = peekNextEventKey();
            assert(isValidStreamIndex(eventKey.streamIndex)); // Next one should be good
        }
    }
}

XEFEvent XEFReader::readDataEvent()
{
    if(endOfStream)
    {
        return XEFEvent();
    }

    EventKey eventKey = readNextEventKey();

    // cant use nullptr directly here, since an actual object with storage is needed I think?
    //      todo: double check
    std::shared_ptr<XEFStream> eventStream = nullStream;
    if(streams.find(eventKey.streamIndex) != streams.end())
    {
        eventStream = streams[eventKey.streamIndex];
    }

    int32_t dataSize;
    read<int32_t>(&dataSize);
    int64_t relativeTimeTicks;
    read<int64_t>(&relativeTimeTicks);
    // skip unknown
    uint32_t unknown;
    read<uint32_t>(&unknown);
    int32_t fullDataSize;
    read<int32_t>(&fullDataSize);

    // tag
    std::shared_ptr<uint8_t[]> tagData = nullptr;
    int frameIndex = 0;

    if(!eventStream->isNull)
    {
        if(eventStream->tagSize > 0)
        {
            tagData = std::shared_ptr<uint8_t[]>(new uint8_t[eventStream->tagSize]);
            file.read(reinterpret_cast<char*>(tagData.get()), eventStream->tagSize);

            if(eventStream->tagSize == 4)
            {
                frameIndex = *reinterpret_cast<int32_t*>(tagData.get());
            }
        }
        assert(!eventStream->isCompressed() || fullDataSize != dataSize);
    }
    else
    {
        tagData = std::shared_ptr<uint8_t[]>(new uint8_t[DataConstants::EVENT_DEFAULT_TAG_SIZE]);
        file.read(reinterpret_cast<char*>(tagData.get()), DataConstants::EVENT_DEFAULT_TAG_SIZE);
    }

    std::shared_ptr<uint8_t[]> eventData{new uint8_t[dataSize]};
    file.read(reinterpret_cast<char*>(eventData.get()), dataSize);

    return XEFEvent(eventStream, frameIndex, relativeTimeTicks, fullDataSize, tagData, eventData, unknown);
}

XEFStream XEFReader::readStreamDescription()
{
    XEFEvent nameEvent = readDataEvent();

    int16_t streamIndex =
        *reinterpret_cast<int16_t*>(nameEvent.tagData.get() + DataConstants::STREAM_INDEX_OFFSET);
    int16_t streamFlags =
        *reinterpret_cast<int16_t*>(nameEvent.tagData.get() + DataConstants::STREAM_FLAGS_OFFSET);
    bool compressed = (streamFlags & DataConstants::FLAG_COMPRESSED) == DataConstants::FLAG_COMPRESSED;
    assert(compressed == false);

    GUID dataTypeID;
    memcpy(
        &dataTypeID,
        nameEvent.tagData.get() + DataConstants::STREAM_TYPID_OFFSET,
        DataConstants::STREAM_TYPID_SIZE);

    // asserted that compressed is false, so can take raw event data here
    //      otherwise would have to decompress in case eventdata is compressed
    std::wstring dataTypeName{(const wchar_t*)nameEvent.rawEventData.get()};
    int16_t tagSize =
        *reinterpret_cast<int16_t*>(nameEvent.rawEventData.get() + DataConstants::STREAM_TAGSIZE_OFFSET);

    if(compressed)
    {
        file.seekg(DataConstants::ARC_STREAM_EXTRA_UNK_SIZE, std::ios_base::cur);
    }

    XEFEvent guidEvent = readDataEvent();
    GUID semanticID;
    memcpy(
        &semanticID,
        nameEvent.tagData.get() + DataConstants::STREAM_SEMID_OFFSET,
        DataConstants::STREAM_SEMID_SIZE);

    return XEFStream(streamIndex, streamFlags, tagSize, dataTypeName, dataTypeID, semanticID);
}