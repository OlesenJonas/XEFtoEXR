#include <cassert>
#include <iostream>
#include <string>

#include "ColorFrame.hpp"
#include "Constants.hpp"
#include "XEFReader.hpp"

int main()
{
    std::string path = "C:/Users/jonas/Documents/GitHub/vrtanz/KinectRecorder/KinectRecorder/misc/out/move2.xef";

    XEFReader reader{path};

    int64_t lastEventTime = 0;
    ColorFrame lastColorFrame;

    XEFEvent event;
    while(!(event = reader.getNextEvent()).isNull)
    {
        assert(event.relativeTimeTicks >= lastEventTime);
        lastEventTime = event.relativeTimeTicks;

        if(event.getEventStreamDataTypeID() == StreamDataTypeIds::UncompressedColor)
        {
            lastColorFrame.replacePixelsWithNewUncompressedColorEvent(event);
        }

        // if(...)
        // {
        // }
    }

    std::cout << "Done processing!" << std::endl;
}