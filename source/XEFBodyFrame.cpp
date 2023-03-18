#include "XEFBodyFrame.hpp"
#include <stdint.h>

const char* XEFJointTypeStrings[] = {
    "SpineBase",   "SpineMid",      "Neck",         "Head",       "ShoulderLeft", "ElbowLeft", "WristLeft",
    "HandLeft",    "ShoulderRight", "ElbowRight",   "WristRight", "HandRight",    "HipLeft",   "KneeLeft",
    "AnkleLeft",   "FootLeft",      "HipRight",     "KneeRight",  "AnkleRight",   "FootRight", "SpineShoulder",
    "HandTipLeft", "ThumbLeft",     "HandTipRight", "ThumbRight",
};

void XEFBodyData::fillFromByteStream(ByteStream& stream)
{
    // todo: vector memory is contiguous, so read all at once?
    for(int i = 0; i < int32_t(XEFJointType::SIZE); i++)
    {
        stream.read<XEFVector>(&SkeletonJointPositions[i]);
    }
    for(int i = 0; i < int32_t(XEFJointType::SIZE); i++)
    {
        stream.read<XEFVector>(&SkeletonJointOrientations[i]);
    }
    for(int i = 0; i < int32_t(XEFJointType::SIZE); i++)
    {
        stream.read<XEFTrackingState>(&SkeletonJointPositionTrackingStates[i]);
    }

    stream.read<XEFExpressionData>(&ExpressionData);

    stream.read<XEFHandData>(&HandDataLeft);
    stream.read<XEFHandData>(&HandDataRight);

    int32_t pad;
    stream.read<int32_t>(&pad);

    stream.read<uint64_t>(&TrackingID);
    stream.read<XEFBodyTrackingState>(&TrackingState);

    stream.read<XEFLeanData>(&LeanData);
    stream.read<uint32_t>(&QualityFlags);
}

void XEFBodyFrame::fillFromByteStream(ByteStream stream)
{
    stream.read<XEFVector>(&FloorClipPlane);
    stream.read<XEFVector>(&Up);

    for(int i = 0; i < BODY_DATA_SIZE; i++)
    {
        BodyData[i].fillFromByteStream(stream);
    }

    stream.read<uint32_t>(&QualityFlags);
    int32_t padding;
    stream.read<int32_t>(&padding);
    stream.read<int32_t>(&padding);
    stream.read<int32_t>(&padding);
}