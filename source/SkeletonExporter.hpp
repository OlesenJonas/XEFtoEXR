#pragma once

#include <filesystem>

#include "XEFBodyFrame.hpp"

struct SkeletonExporter
{
    bool saveSkeleton(XEFBodyFrame& bodyFrame, std::filesystem::path outputPath);
};