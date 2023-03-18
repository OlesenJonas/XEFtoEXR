# XEFtoEXR

A C++ port/adaption/extension of https://github.com/Isaac-W/KinectXEFTools.

Extracts color and depth frames from an XEF file.

Then aligns them using libfreenect2s registration.

This requires knowing the intrinsic parameters of the camera which can also be retrieved using libfreenect2, but is not part of this program.

------

Theres currently no real error handling, just asserts in debug builds.

------

How to use:


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