#include "PointCloudExporter.hpp"

PointCloudExporter::PointCloudExporter(
    libfreenect2::Freenect2Device::ColorCameraParams colorParams,
    libfreenect2::Freenect2Device::IrCameraParams irParams)
    : registration(new libfreenect2::Registration(irParams, colorParams))
{
    pointcloudR.resize(512 * 424);
    pointcloudG.resize(512 * 424);
    pointcloudB.resize(512 * 424);
    pointcloudX.resize(512 * 424);
    pointcloudY.resize(512 * 424);
    pointcloudZ.resize(512 * 424);

    image_ptr[0] = &(pointcloudR.at(0)); // R
    image_ptr[1] = &(pointcloudG.at(0)); // G
    image_ptr[2] = &(pointcloudB.at(0)); // B
    image_ptr[3] = &(pointcloudX.at(0)); // X
    image_ptr[4] = &(pointcloudY.at(0)); // Y
    image_ptr[5] = &(pointcloudZ.at(0)); // Z

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

    InitEXRImage(&image);
    image.num_channels = 3;
    image.images = (unsigned char**)image_ptr;
    image.width = 512;
    image.height = 424;
}

PointCloudExporter::~PointCloudExporter()
{
    delete registration;
    free(header.channels);
    free(header.pixel_types);
    free(header.requested_pixel_types);
}

bool PointCloudExporter::savePointCloud(
    libfreenect2::Frame* rgbFrame, libfreenect2::Frame* depthFrame, std::filesystem::path outputPath)
{
    registration->apply(rgbFrame, depthFrame, &undistorted, &registered);

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
    int ret = SaveEXRImageToFile(&image, &header, outputPath.string().c_str(), &err);
    if(ret != TINYEXR_SUCCESS)
    {
        fprintf(stderr, "Save EXR err: %s\n", err);
        FreeEXRErrorMessage(err); // free's buffer for an error message
        return false;
    }
    // printf("Saved exr file. [ %s ] \n", filePath.c_str());

    return true;
}