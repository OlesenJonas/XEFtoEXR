#pragma once

#include <filesystem>

#include "XEF/XEFBodyFrame.hpp"

struct SkeletonExporter
{
    bool saveSkeleton(XEFBodyFrame& bodyFrame, std::filesystem::path outputPath);
};