#include "ColorFrame.hpp"

#include <cassert>

#include "XEF/XEFEvent.hpp"

// https://stackoverflow.com/questions/17892346/how-to-convert-rgb-yuv-rgb-both-ways#:~:text=VLFeat%2C%20or%20ImageMagick.-,2019%20Edit,-%3A%20Here%27s%20the
void YUVtoRGBA(float Y, float U, float V, uint8_t* rgba)
{
    Y -= 16.0f;
    U -= 128.0f;
    V -= 128.0f;
    const float rf = 1.164f * Y + 1.596f * V;
    const float gf = 1.164f * Y - 0.392f * U - 0.813f * V;
    const float bf = 1.164f * Y + 2.017f * U;

    rgba[0] = std::min(std::max(0.0f, rf), 255.0f);
    rgba[1] = std::min(std::max(0.0f, gf), 255.0f);
    rgba[2] = std::min(std::max(0.0f, bf), 255.0f);
    rgba[3] = 255;
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

        YUVtoRGBA(y1, u, v, &rgbPixels.data[pixel1BaseIndex]);
        YUVtoRGBA(y2, u, v, &rgbPixels.data[pixel2BaseIndex]);
    }
    return true;
}