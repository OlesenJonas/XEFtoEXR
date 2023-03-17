#include <cassert>
#include <iostream>
#include <string>
#include <tinyEXR/tinyexr.h>

#include <libfreenect2/registration.hpp>

#include "ColorFrame.hpp"
#include "Constants.hpp"
#include "DepthFrame.hpp"
#include "XEFReader.hpp"
#include "jsonConversion.hpp"

using json = nlohmann::json;

int main()
{
    std::string path = "C:/Users/jonas/Documents/GitHub/vrtanz/KinectRecorder/KinectRecorder/misc/out/move2.xef";
    std::string jsonPath =
        "C:/Users/jonas/Documents/GitHub/vrtanz/KinectRecorder/KinectRecorder/misc/SensorData.json";
    std::string deviceName = "2";

    std::ifstream jsonFile(jsonPath);
    assert(jsonFile);
    json j;
    jsonFile >> j;
    jsonFile.close();

    XEFReader reader{path};

    int64_t lastEventTime = 0;
    ColorFrame lastColorFrame;
    DepthFrame lastDepthFrame;

    const auto colorCameraParams = colorCameraParamsFromJson(j[deviceName]["ColorCameraParams"]);
    const auto irCameraParams = irCameraParamsFromJson(j[deviceName]["IrCameraParams"]);
    auto* registration = new libfreenect2::Registration(irCameraParams, colorCameraParams);
    libfreenect2::Frame undistorted(512, 424, 4);
    libfreenect2::Frame registered(512, 424, 4);

    XEFEvent event;
    while(!(event = reader.getNextEvent()).isNull)
    {
        assert(event.relativeTimeTicks >= lastEventTime);
        lastEventTime = event.relativeTimeTicks;

        if(event.getEventStreamDataTypeID() == StreamDataTypeIds::UncompressedColor)
        {
            lastColorFrame.replacePixelsWithNewUncompressedColorEvent(event);
        }

        if(event.getEventStreamDataTypeID() == StreamDataTypeIds::Depth)
        {
            lastDepthFrame.replacePixelsWithNewDepthEvent(event);
        }

        // if(...)
        // {
        // }
    }

    // for now just use the last two available frames for testing purposes
    registration->apply(&lastColorFrame.rgbPixels, &lastDepthFrame.floatPixels, &undistorted, &registered);

    delete registration;

    std::cout << "Done processing!" << std::endl;
}