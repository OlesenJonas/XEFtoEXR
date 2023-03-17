#include "jsonConversion.hpp"
#include "libfreenect2/libfreenect2.hpp"

#define fromJson(member) j.at(#member).get_to(params.member)

libfreenect2::Freenect2Device::ColorCameraParams colorCameraParamsFromJson(const nlohmann::json& j)
{
    libfreenect2::Freenect2Device::ColorCameraParams params{};
    // j.at("fx").get_to(params.fx);
    fromJson(fx);
    fromJson(fy);
    fromJson(cx);
    fromJson(cy);

    fromJson(shift_d);
    fromJson(shift_m);

    fromJson(mx_x3y0);
    fromJson(mx_x0y3);
    fromJson(mx_x2y1);
    fromJson(mx_x1y2);
    fromJson(mx_x2y0);
    fromJson(mx_x0y2);
    fromJson(mx_x1y1);
    fromJson(mx_x1y0);
    fromJson(mx_x0y1);
    fromJson(mx_x0y0);

    fromJson(my_x3y0);
    fromJson(my_x0y3);
    fromJson(my_x2y1);
    fromJson(my_x1y2);
    fromJson(my_x2y0);
    fromJson(my_x0y2);
    fromJson(my_x1y1);
    fromJson(my_x1y0);
    fromJson(my_x0y1);
    fromJson(my_x0y0);

    return params;
}

libfreenect2::Freenect2Device::IrCameraParams irCameraParamsFromJson(const nlohmann::json& j)
{
    libfreenect2::Freenect2Device::IrCameraParams params{};
    fromJson(fx);
    fromJson(fy);
    fromJson(cx);
    fromJson(cy);

    fromJson(k1);
    fromJson(k2);

    fromJson(k3);
    fromJson(p1);
    fromJson(p2);

    return params;
}

#undef fromJson