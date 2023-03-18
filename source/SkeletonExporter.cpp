#include "SkeletonExporter.hpp"

#include <format>
#include <fstream>
#include <ios>

bool SkeletonExporter::saveSkeleton(XEFBodyFrame& bodyFrame, std::filesystem::path outputPath)
{
    std::ofstream outFile(outputPath, std::ios_base::out);

    for(int i = 0; i < bodyFrame.BodyData.size(); i++)
    {
        outFile << std::format("Body {}\n", i);
        if(bodyFrame.BodyData[i].TrackingState == XEFBodyTrackingState::TRACKED)
        {
            for(int j = 0; j < int32_t(XEFJointType::SIZE); j++)
            {
                outFile << std::format(
                    "{}\n{}\n{},{},{},{}\n{},{},{},{}\n",
                    XEFJointTypeStrings[j],
                    int(bodyFrame.BodyData[i].SkeletonJointPositionTrackingStates[j]),

                    bodyFrame.BodyData[i].SkeletonJointPositions[j].elements[0],
                    bodyFrame.BodyData[i].SkeletonJointPositions[j].elements[1],
                    bodyFrame.BodyData[i].SkeletonJointPositions[j].elements[2],
                    bodyFrame.BodyData[i].SkeletonJointPositions[j].elements[3],

                    bodyFrame.BodyData[i].SkeletonJointOrientations[j].elements[0],
                    bodyFrame.BodyData[i].SkeletonJointOrientations[j].elements[1],
                    bodyFrame.BodyData[i].SkeletonJointOrientations[j].elements[2],
                    bodyFrame.BodyData[i].SkeletonJointOrientations[j].elements[3]);
            }
        }
        outFile << "\n" << std::endl;
    }

    outFile.close();
    return true;
}
