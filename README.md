# XEFtoEXR

A C++ port/adaption of https://github.com/Isaac-W/KinectXEFTools.

Extracts skeleton information, color-, and depth frames from a Kinect XEF file. The frames get merged into pointclouds using [libfreenect2](https://github.com/OpenKinect/libfreenect2)s registration and saved as an exr image sequence using [tinyexr](https://github.com/syoyo/tinyexr).

This requires knowing the intrinsic parameters of the camera which can also be retrieved using libfreenect2, but is not part of this program.

------

#### Theres currently no real error handling, just asserts in debug builds.

------

Usage:  
- `XEFtoEXR.exe XEF_PATH JSON_PATH OBJECT_NAME`

Example:  
- `XEFtoEXR.exe F:\test.xef F:\SensorData.json MainCamera`

Where the .json file contains the same parameters as the `ColorCameraParams` and `IrCameraParams` shown [here](https://github.com/OpenKinect/libfreenect2/blob/master/include/libfreenect2/libfreenect2.hpp) either directly, or inside another object. If the parameters are stored inside an object, OBJECT_NAME should be the name of that object.

[Example](#JSONExample)


------

Resulting skeleton file:

- CSV Format

- "Body N" indicates either the start of skeleton information, or that that skeleton isnt being tracked if the following line is empty

- Skeleton information consists multiple 4-line groups (always in the same order), consisting of:
	- Name of joint
	- Tracking state (0: not tracked, 1: inferred, 2: tracked)
	- Position as 4 floats
	- Orientation as 4 floats

------

Building from source requires:

- CMake

- Compiler with C++20 support

(Tested to work in vscode using CMake tools and Clang 15)

-------
<a name="JSONExample"></a>
Parameter JSON Example

```
{
    "MainCamera": {  
        "ColorCameraParams": {  
            "cx": 959.5,  
            "cy": 539.5,  
            "fx": 1081.3720703125,  
            "fy": 1081.3720703125,  
            "mx_x0y0": 0.15437400341033936,  
            "mx_x0y1": -0.0021031571086496115,  
            "mx_x0y2": 0.00011753859871532768,  
            "mx_x0y3": 6.395136733772233e-05,  
            "mx_x1y0": 0.6361346244812012,  
            "mx_x1y1": -0.0008770275162532926,  
            "mx_x1y2": 0.0004076508048456162,  
            "mx_x2y0": 0.0002103597071254626,  
            "mx_x2y1": -1.9841419998556376e-05,  
            "mx_x3y0": 0.0005796676850877702,  
            "my_x0y0": -0.03866000846028328,  
            "my_x0y1": 0.634756326675415,  
            "my_x0y2": -0.0008559309062547982,  
            "my_x0y3": 0.000933802395593375,  
            "my_x1y0": 0.0018689970020204782,  
            "my_x1y1": 0.00012271609739400446,  
            "my_x1y2": -3.733723860932514e-05,  
            "my_x2y0": -7.934760651551187e-05,  
            "my_x2y1": 0.0005619486910291016,  
            "my_x3y0": 2.5257049856008962e-05,  
            "shift_d": 863.0,  
            "shift_m": 52.0  
        },
        "IrCameraParams": {
            "cx": 259.2738037109375,  
            "cy": 209.47959899902344,  
            "fx": 368.9435119628906,  
            "fy": 368.9435119628906,\
            "k1": 0.08521547168493271,  
            "k2": -0.27218571305274963,  
            "k3": 0.10060829669237137,  
            "p1": 0.0,  
            "p2": 0.0  
        }  
    }  
}  
```
