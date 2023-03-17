#include <cassert>
#include <filesystem>
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
    std::filesystem::path inputFilePath =
        "C:/Users/jonas/Documents/GitHub/vrtanz/KinectRecorder/KinectRecorder/misc/out/test2.xef";
    std::filesystem::path jsonPath =
        "C:/Users/jonas/Documents/GitHub/vrtanz/KinectRecorder/KinectRecorder/misc/SensorData.json";
    std::string objectName = "2";

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
    auto* registration = new libfreenect2::Registration(irCameraParams, colorCameraParams);
    libfreenect2::Frame undistorted(512, 424, 4);
    libfreenect2::Frame registered(512, 424, 4);

    int64_t lastEventTime = 0;
    ColorFrame lastColorFrame;
    DepthFrame lastDepthFrame;

    // Exr setup
    std::vector<float> pointcloudR(512 * 424);
    std::vector<float> pointcloudG(512 * 424);
    std::vector<float> pointcloudB(512 * 424);
    std::vector<float> pointcloudX(512 * 424);
    std::vector<float> pointcloudY(512 * 424);
    std::vector<float> pointcloudZ(512 * 424);

    float* image_ptr[6];
    image_ptr[0] = &(pointcloudR.at(0)); // R
    image_ptr[1] = &(pointcloudG.at(0)); // G
    image_ptr[2] = &(pointcloudB.at(0)); // B
    image_ptr[3] = &(pointcloudX.at(0)); // X
    image_ptr[4] = &(pointcloudY.at(0)); // Y
    image_ptr[5] = &(pointcloudZ.at(0)); // Z

    EXRHeader header;
    InitEXRHeader(&header);
    header.num_channels = 6;
    header.channels = (EXRChannelInfo*)malloc(sizeof(EXRChannelInfo) * header.num_channels);
    strncpy(header.channels[0].name, "R", 255);
    header.channels[0].name[strlen("R")] = '\0';
    strncpy(header.channels[1].name, "G", 255);
    header.channels[1].name[strlen("G")] = '\0';
    strncpy(header.channels[2].name, "B", 255);
    header.channels[2].name[strlen("B")] = '\0';
    strncpy(header.channels[3].name, "X", 255);
    header.channels[3].name[strlen("X")] = '\0';
    strncpy(header.channels[4].name, "Y", 255);
    header.channels[4].name[strlen("Y")] = '\0';
    strncpy(header.channels[5].name, "Z", 255);
    header.channels[5].name[strlen("Z")] = '\0';

    header.pixel_types = (int*)malloc(sizeof(int) * header.num_channels);
    header.requested_pixel_types = (int*)malloc(sizeof(int) * header.num_channels);
    for(int i = 0; i < header.num_channels; i++)
    {
        header.pixel_types[i] = TINYEXR_PIXELTYPE_FLOAT; // pixel type of input image
        header.requested_pixel_types[i] =
            TINYEXR_PIXELTYPE_HALF; // pixel type of output image to be stored in .EXR
        // header.requested_pixel_types[i] =
        // TINYEXR_PIXELTYPE_FLOAT; // pixel type of output image to be stored in .EXR
    }
    header.compression_type = TINYEXR_COMPRESSIONTYPE_NONE;

    EXRImage image;
    InitEXRImage(&image);
    image.num_channels = 3;
    image.images = (unsigned char**)image_ptr;
    image.width = 512;
    image.height = 424;

    const auto createAndSavePointCloud = [&](const std::filesystem::path& filePath) -> void
    {
        // todo: x,y,z can be NaN, not sure if that messes something up in EXR export / TD import
        //       atm that doesnt seem to be the case
        float x = 0;
        float y = 0;
        float z = 0;
        float packedRGB = 0;
        for(int i = 0; i < 424; i++)
        {
            for(int j = 0; j < 512; j++)
            {
                registration->getPointXYZRGB(&undistorted, &registered, i, j, x, y, z, packedRGB);
                const uint8_t* unpackedRGB = reinterpret_cast<uint8_t*>(&packedRGB);
                uint8_t b = unpackedRGB[0];
                uint8_t g = unpackedRGB[1];
                uint8_t r = unpackedRGB[2];

                pointcloudX[i * 512 + j] = x;
                pointcloudY[i * 512 + j] = y;
                pointcloudZ[i * 512 + j] = z;
                pointcloudR[i * 512 + j] = float(r) / 255.0f;
                pointcloudG[i * 512 + j] = float(g) / 255.0f;
                pointcloudB[i * 512 + j] = float(b) / 255.0f;
            }
        }

        const char* err = nullptr;
        int ret = SaveEXRImageToFile(&image, &header, filePath.string().c_str(), &err);
        if(ret != TINYEXR_SUCCESS)
        {
            fprintf(stderr, "Save EXR err: %s\n", err);
            FreeEXRErrorMessage(err); // free's buffer for an error message
            return;
        }
        // printf("Saved exr file. [ %s ] \n", filePath.c_str());
    };
    const auto filePathFromIndex = [&outputDirectory](uint32_t index) -> std::filesystem::path
    {
        std::string outFileBase = "pointcloud000000";
        const std::string outFileExtension = ".exr";
        // const std::string outFileExtension = ".ply";
        std::sprintf(&outFileBase[10], "%06u", index);

        return outputDirectory / (outFileBase + outFileExtension);
    };

    XEFEvent event;
    int absoluteFrameIndex = 0;
    while(!(event = reader.getNextEvent()).isNull)
    {
        assert(event.relativeTimeTicks >= lastEventTime);
        lastEventTime = event.relativeTimeTicks;

        if(event.getEventStreamDataTypeID() == StreamDataTypeIds::Depth)
        {
            auto diff = event.relativeTimeTicks - lastDepthFrame.timeInTicks;
            // if this fails a frame was most likely dropped
            assert((abs(diff - 333333) < 10000) || event.eventIndex == 0);
            lastDepthFrame.replacePixelsWithNewDepthEvent(event);
            lastDepthFrame.timeInTicks = event.relativeTimeTicks;
        }

        if(event.getEventStreamDataTypeID() == StreamDataTypeIds::UncompressedColor)
        {
            auto diff = event.relativeTimeTicks - lastColorFrame.timeInTicks;
            // if this fails a frame was most likely dropped
            assert((abs(diff - 333333) < 10000) || event.eventIndex == 0);
            lastColorFrame.replacePixelsWithNewUncompressedColorEvent(event);
            lastColorFrame.timeInTicks = event.relativeTimeTicks;

            // Color frames are always the last frame in an event, so we can produce another output frame now
            registration->apply(&lastColorFrame.rgbPixels, &lastDepthFrame.floatPixels, &undistorted, &registered);
            createAndSavePointCloud(filePathFromIndex(absoluteFrameIndex));
            absoluteFrameIndex++;
        }
    }

    delete registration;

    std::cout << "Done processing!" << std::endl;
}