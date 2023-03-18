#include "ColorFrame.hpp"

#include <cassert>

#include "XEF/XEFEvent.hpp"

bool ColorFrame::replacePixelsWithNewUncompressedColorEvent(const XEFEvent& event)
{
    assert(event.getEventStreamDataTypeID() == StreamDataTypeIds::UncompressedColor);
    // one YUYV packet contains two pixels
    constexpr auto amountOfPackets = 1920 * 1080 / 2;
    assert(amountOfPackets * sizeof(YUYV422Packet) == event.eventDataSize);
    auto const* yuvPackets = reinterpret_cast<YUYV422Packet*>(event.rawEventData.get());

    // unpack yuyv packing + convert yuv to rgb
    //  todo: optimize!

    // https://stackoverflow.com/questions/17892346/how-to-convert-rgb-yuv-rgb-both-ways#:~:text=VLFeat%2C%20or%20ImageMagick.-,2019%20Edit,-%3A%20Here%27s%20the
    auto YUVtoRGBA = [](double Y, double U, double V) -> RGBA
    {
        Y -= 16.0;
        U -= 128.0;
        V -= 128.0;
        const auto r = 1.164 * Y + 1.596 * V;
        const auto g = 1.164 * Y - 0.392 * U - 0.813 * V;
        const auto b = 1.164 * Y + 2.017 * U;

        RGBA pixel;
        pixel.rgba[0] = std::min(std::max(0.0, r), 255.0);
        pixel.rgba[1] = std::min(std::max(0.0, g), 255.0);
        pixel.rgba[2] = std::min(std::max(0.0, b), 255.0);
        pixel.rgba[3] = 255;

        return pixel;
    };
    for(int i = 0; i < amountOfPackets; i++)
    {
        const uint8_t& y1 = yuvPackets[i].yuyv[0];
        const uint8_t& u = yuvPackets[i].yuyv[1];
        const uint8_t& y2 = yuvPackets[i].yuyv[2];
        const uint8_t& v = yuvPackets[i].yuyv[3];

        const RGBA pixel1 = YUVtoRGBA(y1, u, v);
        const RGBA pixel2 = YUVtoRGBA(y2, u, v);

        auto pixel1BaseIndex = (2 * i) * 4;
        auto pixel2BaseIndex = (2 * i + 1) * 4;

        // just memcpy?, or optimize by passing RGBA as references to YUVtoRGB function
        rgbPixels.data[pixel1BaseIndex + 0] = pixel1.rgba[0];
        rgbPixels.data[pixel1BaseIndex + 1] = pixel1.rgba[1];
        rgbPixels.data[pixel1BaseIndex + 2] = pixel1.rgba[2];
        rgbPixels.data[pixel1BaseIndex + 3] = pixel1.rgba[3];

        rgbPixels.data[pixel2BaseIndex + 0] = pixel2.rgba[0];
        rgbPixels.data[pixel2BaseIndex + 1] = pixel2.rgba[1];
        rgbPixels.data[pixel2BaseIndex + 2] = pixel2.rgba[2];
        rgbPixels.data[pixel2BaseIndex + 3] = pixel2.rgba[3];
    }
    return true;
}