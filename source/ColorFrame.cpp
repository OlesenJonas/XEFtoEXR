#include "ColorFrame.hpp"

#include <cassert>

#include "XEF/XEFEvent.hpp"

// https://stackoverflow.com/questions/17892346/how-to-convert-rgb-yuv-rgb-both-ways#:~:text=VLFeat%2C%20or%20ImageMagick.-,2019%20Edit,-%3A%20Here%27s%20the
void YUVtoRGBA(uint8_t* YUYV, uint8_t* rgbaPair)
{
    float Y1 = YUYV[0] - 16.0f;
    float U = YUYV[1] - 128.0f;
    float Y2 = YUYV[2] - 16.0f;
    float V = YUYV[3] - 128.0f;

    const float r1 = 1.164f * Y1 + 1.596f * V;
    const float g1 = 1.164f * Y1 - 0.392f * U - 0.813f * V;
    const float b1 = 1.164f * Y1 + 2.017f * U;

    const float r2 = 1.164f * Y1 + 1.596f * V;
    const float g2 = 1.164f * Y1 - 0.392f * U - 0.813f * V;
    const float b2 = 1.164f * Y1 + 2.017f * U;

    rgbaPair[0] = std::min(std::max(0.0f, r1), 255.0f);
    rgbaPair[1] = std::min(std::max(0.0f, g1), 255.0f);
    rgbaPair[2] = std::min(std::max(0.0f, b1), 255.0f);
    rgbaPair[3] = 255;
    rgbaPair[4] = std::min(std::max(0.0f, r2), 255.0f);
    rgbaPair[5] = std::min(std::max(0.0f, g2), 255.0f);
    rgbaPair[6] = std::min(std::max(0.0f, b2), 255.0f);
    rgbaPair[7] = 255;
};

bool ColorFrame::replacePixelsWithNewUncompressedColorEvent(const XEFEvent& event)
{
    assert(event.getEventStreamDataTypeID() == StreamDataTypeIds::UncompressedColor);
    // one YUYV packet contains two pixels
    constexpr auto amountOfPackets = 1920 * 1080 / 2;
    assert(amountOfPackets * sizeof(YUYV422Packet) == event.eventDataSize);
    auto const* yuvPackets = reinterpret_cast<YUYV422Packet*>(event.rawEventData.get());

    // unpack yuyv packing + convert yuv to rgb

    for(int i = 0; i < amountOfPackets; i++)
    {
        const uint8_t& y1 = yuvPackets[i].yuyv[0];
        const uint8_t& u = yuvPackets[i].yuyv[1];
        const uint8_t& y2 = yuvPackets[i].yuyv[2];
        const uint8_t& v = yuvPackets[i].yuyv[3];

        auto pixel1BaseIndex = (2 * i) * 4;
        auto pixel2BaseIndex = (2 * i + 1) * 4;

        YUVtoRGBA((uint8_t*)&yuvPackets[i], &rgbPixels.data[pixel1BaseIndex]);
    }
    return true;
}