# XEFtoEXR

A C++ port/adaption/extension of https://github.com/Isaac-W/KinectXEFTools.

Extracts color and depth frames from an XEF file.

Then tries to align them temporally and also in 3D using libfreenect2s registration.

This requires knowing the intrinsic parameters of the camera which can also be retrieved using libfreenect2, but is not part of this program.

------

Theres currently no real error handling, just asserts in debug builds.

------

Building from source requires:

- CMake

- Compiler with C++20 support

- vcpkg (and root folder location saved as environment variable VCPKG_ROOT)
	- currently used for easily adding libfreenect2 to the project. I dont like pulling all of it just to use the registration, but it works for now

(Tested to work in vscode using CMake tools and Clang 15)