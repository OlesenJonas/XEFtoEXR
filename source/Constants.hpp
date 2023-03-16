#pragma once

#include <stdint.h>
#include <string_view>

#include "GUID.hpp"

struct NuiConstants
{
    static constexpr uint32_t STREAM_FRAME_LIMIT_MAXIMUM = 15;
    static constexpr uint32_t AUDIO_FRAME_VERSION_MINOR_MASK = 65535;
    static constexpr uint32_t AUDIO_FRAME_VERSION_MAJOR_MASK = 4294901760;
    static constexpr uint32_t AUDIO_FRAME_VERSION = 65536;
    static constexpr uint32_t AUDIO_RESERVED_BYTE_ARRAY_SIZE = 1024;
    static constexpr uint32_t AUDIO_NUM_SPK = 8;
    static constexpr uint32_t AUDIO_NUM_MIC = 4;
    static constexpr uint32_t AUDIO_SAMPLES_PER_SUBFRAME = 256;
    static constexpr uint32_t AUDIO_SAMPLERATE = 16000;
    static constexpr uint32_t AUDIO_MAX_SUBFRAMES = 8;
    static constexpr uint32_t MAX_AUDIO_FRAME_SIZE = 115344;
    static constexpr uint32_t STREAM_BODY_INDEX_HEIGHT = 424;
    static constexpr uint32_t STREAM_COLOR_HEIGHT = 1080;
    static constexpr uint32_t STREAM_COLOR_WIDTH = 1920;
    static constexpr uint32_t STREAM_IR_HEIGHT = 424;
    static constexpr uint32_t STREAM_IR_WIDTH = 512;
    static constexpr uint32_t STREAM_DEPTH_HEIGHT = 424;
    static constexpr uint32_t STREAM_DEPTH_WIDTH = 512;
    static constexpr uint8_t BODY_INDEX_BACKGROUND = 255;
    static constexpr uint32_t BODY_INVALID_TRACKING_ID = 0;
    static constexpr uint32_t BODY_COUNT = 6;
    static constexpr uint32_t STREAM_BODY_INDEX_WIDTH = 512;
};

// In the C# version these are GUIDs but for now this works
struct StreamDataTypeIds
{
    static constexpr GUID UncompressedColor{
        .byteArray = {125, 214, 160, 43, 17, 190, 52, 69, 148, 68, 63, 178, 26, 224, 240, 139}};
    static constexpr GUID LongExposureIr{
        .byteArray = {142, 217, 6, 126, 113, 210, 31, 74, 155, 253, 102, 72, 167, 0, 219, 117}};
    static constexpr GUID CompressedColor{
        .byteArray = {220, 20, 57, 10, 22, 59, 225, 17, 170, 195, 0, 30, 79, 213, 140, 15}};
    static constexpr GUID RawIr{
        .byteArray = {226, 20, 57, 10, 22, 59, 225, 17, 170, 195, 0, 30, 79, 213, 140, 15}};
    static constexpr GUID BodyIndex{
        .byteArray = {172, 255, 130, 223, 51, 181, 56, 68, 149, 74, 104, 106, 30, 32, 244, 170}};
    static constexpr GUID Body{
        .byteArray = {121, 81, 196, 160, 104, 81, 117, 72, 167, 92, 248, 241, 118, 15, 99, 124}};
    static constexpr GUID Depth{
        .byteArray = {214, 20, 57, 10, 22, 59, 225, 17, 170, 195, 0, 30, 79, 213, 140, 15}};
    static constexpr GUID Ir{.byteArray = {215, 20, 57, 10, 22, 59, 225, 17, 170, 195, 0, 30, 79, 213, 140, 15}};
    static constexpr GUID Audio{
        .byteArray = {189, 122, 124, 120, 110, 159, 133, 74, 141, 103, 99, 101, 255, 128, 204, 105}};
    static constexpr GUID Properties{
        .byteArray = {47, 163, 131, 128, 180, 215, 155, 68, 153, 185, 68, 198, 252, 217, 117, 112}};
    static constexpr GUID Null{.byteArray = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};
};

struct DataConstants
{
    static constexpr int16_t FLAG_COMPRESSED = 0x0001;

    static constexpr int32_t METADATA_TAG_SIZE = 0x18;

    static constexpr int32_t STREAM_COUNT_ADDRESS = 0xC;

    static constexpr int32_t STREAM_COUNT_SIZE = 4; // int

    static constexpr int32_t EVENT_DEFAULT_TAG_SIZE = 24;

    // Stream description data

    static constexpr int32_t STREAM_INDEX_OFFSET = 0;

    static constexpr int32_t STREAM_FLAGS_OFFSET = 2;

    static constexpr int32_t STREAM_TYPID_OFFSET = 4;

    static constexpr int32_t STREAM_TYPID_SIZE = 16; // guid

    static constexpr int32_t STREAM_NAME_SIZE = 256; // wstr

    static constexpr int32_t STREAM_TAGSIZE_OFFSET = 258;

    static constexpr int32_t STREAM_SEMID_OFFSET = 82;

    static constexpr int32_t STREAM_SEMID_SIZE = 16; // guid

    static constexpr int32_t ARC_STREAM_EXTRA_UNK_SIZE = 8;

    static constexpr int16_t EVENT_UNKRECORD_INDEX = -1;

    static constexpr int32_t FOOTER_INDEX_SIZE = 2; // ushort

    static constexpr int32_t FOOTER_SIZE = 0x6B0;
};