#pragma once

#include <json/json.hpp>
#include <libfreenect2/libfreenect2.hpp>

libfreenect2::Freenect2Device::ColorCameraParams colorCameraParamsFromJson(const nlohmann::json& json);
libfreenect2::Freenect2Device::IrCameraParams irCameraParamsFromJson(const nlohmann::json& json);