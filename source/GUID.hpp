#pragma once

#include <stdint.h>

struct GUIDByteArray
{
    uint8_t bytes[16];
};
// from windows guiddef.h
// changed first two members to signed types to match C#s private members
struct GUIDParts
{
    int32_t Data1;
    int16_t Data2;
    uint16_t Data3;
    uint8_t Data4[8];
};

static_assert(sizeof(GUIDByteArray) == sizeof(GUIDParts));

union GUID
{
    GUIDParts parts;
    GUIDByteArray byteArray;
};