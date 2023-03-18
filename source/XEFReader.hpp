#pragma once

#include "GUID.hpp"
#include "XEFEvent.hpp"
#include "XEFStream.hpp"

#include <fstream>
#include <ios>
#include <map>
#include <stdint.h>
#include <string>
#include <string_view>

class XEFReader
{
  public:
    explicit XEFReader(std::string_view path);
    ~XEFReader();

    struct EventKey
    {
        int16_t streamIndex;
        int16_t streamFlags;
    };

    template <typename T>
    inline void read(T* ptr)
    {
        file.read(reinterpret_cast<char*>(ptr), sizeof(T));
    }
    inline void skip(std::basic_istream<char>::off_type amount)
    {
        file.seekg(amount, std::ios_base::cur);
    }

    XEFEvent getNextEvent();

  private:
    std::string path;
    std::ifstream file;

    int32_t streamCount = 0;
    std::map<int16_t, std::shared_ptr<XEFStream>> streams;

    bool endOfStream = false;
    int64_t eventStartAdress = -1;

    [[nodiscard]] bool isValidStreamIndex(int16_t index) const;
    EventKey peekNextEventKey();
    EventKey readNextEventKey();
    void readUnknownEvent();
    XEFEvent readDataEvent();
    XEFStream readStreamDescription();
};