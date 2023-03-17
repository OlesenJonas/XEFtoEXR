#include "GUID.hpp"

#include <cstring>

bool GUID::operator==(const GUID& other)
{
    return memcmp(&byteArray.bytes[0], &other.byteArray.bytes[0], sizeof(GUID)) == 0;
}