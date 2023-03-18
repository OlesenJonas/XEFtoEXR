#pragma once

#include <filesystem>
#include <libfreenect2/registration.hpp>
#include <tinyEXR/tinyexr.h>

struct PointCloudExporter
{
    PointCloudExporter(
        libfreenect2::Freenect2Device::ColorCameraParams colorParams,
        libfreenect2::Freenect2Device::IrCameraParams irParams);
    ~PointCloudExporter();

    bool savePointCloud(
        libfreenect2::Frame* rgbFrame, libfreenect2::Frame* depthFrame, std::filesystem::path outputPath);

    // Registration
    libfreenect2::Registration* registration;
    libfreenect2::Frame undistorted{512, 424, 4};
    libfreenect2::Frame registered{512, 424, 4};

    // EXR
    std::vector<float> pointcloudR;
    std::vector<float> pointcloudG;
    std::vector<float> pointcloudB;
    std::vector<float> pointcloudX;
    std::vector<float> pointcloudY;
    std::vector<float> pointcloudZ;
    float* image_ptr[6];
    EXRHeader header;
    EXRImage image;
};