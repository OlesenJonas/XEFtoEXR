#include <iostream>
#include <string>

#include "Constants.hpp"
#include "XEFReader.hpp"

int main()
{
    std::string path = "C:/Users/jonas/Documents/GitHub/vrtanz/KinectRecorder/KinectRecorder/misc/out/move2.xef";

    XEFReader reader{path};

    XEFEvent ev;
    while(!(ev = reader.getNextEvent()).isNull)
    {
    }
}