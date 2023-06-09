#include <cassert>
#include <chrono>
#include <filesystem>
#include <iostream>
#include <string>
#include <tinyEXR/tinyexr.h>

#include <libfreenect2/registration.hpp>

#include "ColorFrame.hpp"
#include "DepthFrame.hpp"
#include "PointCloudExporter.hpp"
#include "SkeletonExporter.hpp"
#include "XEF/Constants.hpp"
#include "XEF/XEFBodyFrame.hpp"
#include "XEF/XEFReader.hpp"
#include "jsonConversion.hpp"

#ifdef _WIN32
    #include <intrin.h>
    #define BREAKPOINT __debugbreak()
#else
    // not tested
    #define BREAKPOINT __builtin_trap()
#endif

using json = nlohmann::json;

int main(int argc, char* argv[])
{
    if(argc < 3 + 1)
    {
        std::cout << "Wrong amount of arguments. Check readme for correct usage!" << std::endl;
        exit(1);
    }

    std::string inputPathString = argv[1];
    std::string jsonPathString = argv[2];
    std::string objectName = argc >= 3 + 1 ? argv[3] : "";

    std::filesystem::path inputFilePath = inputPathString;
    std::filesystem::path jsonPath = jsonPathString;

    auto start = std::chrono::high_resolution_clock::now();

    assert(inputFilePath.extension().string() == ".xef");
    std::filesystem::path baseName = inputFilePath.stem().filename();
    std::filesystem::path outputDirectory = inputFilePath.parent_path() / baseName;
    if(!std::filesystem::exists(outputDirectory))
    {
        std::filesystem::create_directory(outputDirectory);
    }

    std::ifstream jsonFile(jsonPath);
    assert(jsonFile);
    json fullJson;
    jsonFile >> fullJson;
    jsonFile.close();

    const json& jsonObject = objectName.empty() ? fullJson : *fullJson.find(objectName);

    XEFReader reader{inputFilePath.string()};

    const auto colorCameraParams = colorCameraParamsFromJson(jsonObject["ColorCameraParams"]);
    const auto irCameraParams = irCameraParamsFromJson(jsonObject["IrCameraParams"]);
    PointCloudExporter pointCloudExporter{colorCameraParams, irCameraParams};
    SkeletonExporter skeletonExporter;

    int64_t lastEventTime = 0;
    ColorFrame lastColorFrame;
    DepthFrame lastDepthFrame;
    XEFBodyFrame lastBodyFrame;

    const auto getIndexSuffix = [](uint32_t index) -> std::string
    {
        static std::string base = "000000";
        std::sprintf(&base[0], "%06u", index);
        return base;
    };

    XEFEvent event;
    int absoluteFrameIndex = 0;
    while(!(event = reader.getNextEvent()).isNull)
    {
        assert(event.relativeTimeTicks >= lastEventTime);
        lastEventTime = event.relativeTimeTicks;

        if(event.getEventStreamDataTypeID() == StreamDataTypeIds::Body)
        {
            assert(!event.eventStream->isCompressed());
            lastBodyFrame.replaceWithNewBodyEvent(event);
            // lastBodyFrame.fillFromByteStream({
            //     .data = reinterpret_cast<char*>(event.rawEventData.get()),
            //     .size = event.eventDataSize,
            //     .curIndex = 0,
            // });
        }

        if(event.getEventStreamDataTypeID() == StreamDataTypeIds::Depth)
        {
            auto diff = event.relativeTimeTicks - lastDepthFrame.timeInTicks;
            lastDepthFrame.replacePixelsWithNewDepthEvent(event);
            lastDepthFrame.timeInTicks = event.relativeTimeTicks;
        }

        if(event.getEventStreamDataTypeID() == StreamDataTypeIds::UncompressedColor)
        {
            // Check if frames were dropped by calculating the 33ms intervals between the current and last event
            // could also handle cases where only color frames were dropped, but depth frames did not
            //      but considering thats an error state anyways, I dont care
            auto diff = event.relativeTimeTicks - lastColorFrame.timeInTicks;
            int intervals = std::round(diff / 333333.0f);
            if(intervals > 1 && event.eventIndex > 0)
            {
                std::cout << "\r" << intervals - 1 << " frame(s) missing! Filling gap with last received"
                          << std::endl;
                // dont want gaps in the video, so use last received frames to fill
                for(int i = 0; i < intervals - 1; i++)
                {
                    pointCloudExporter.savePointCloud(
                        &lastColorFrame.rgbPixels,
                        &lastDepthFrame.floatPixels,
                        outputDirectory / ("pointcloud" + getIndexSuffix(absoluteFrameIndex) + ".exr"));
                    skeletonExporter.saveSkeleton(
                        lastBodyFrame,
                        outputDirectory / ("skeleton" + getIndexSuffix(absoluteFrameIndex) + ".csv"));
                    absoluteFrameIndex++;
                }
            }
            lastColorFrame.replacePixelsWithNewUncompressedColorEvent(event);
            lastColorFrame.timeInTicks = event.relativeTimeTicks;

            // Color frames are always the last frame in an event, so we can produce another output frame now
            pointCloudExporter.savePointCloud(
                &lastColorFrame.rgbPixels,
                &lastDepthFrame.floatPixels,
                outputDirectory / ("pointcloud" + getIndexSuffix(absoluteFrameIndex) + ".exr"));
            skeletonExporter.saveSkeleton(
                lastBodyFrame, outputDirectory / ("skeleton" + getIndexSuffix(absoluteFrameIndex) + ".csv"));
            absoluteFrameIndex++;
        }

        std::cout << "\r"
                  << "Approx. progress: " << (100 * reader.getProgress()) / reader.fileSize << "%" << std::flush;
    }
    std::cout << std::endl;

    std::cout << "Done processing!" << std::endl;

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Time taken (s): " << (duration.count() / 1000.0) << std::endl;
}