#pragma once

#include <array>
#include <cassert>
#include <stdint.h>

// Very basic stream-like abstraction to enable reading bytes from a byte array
// through simple memcpy
struct ByteStream
{
    char* data;
    int32_t size;
    int32_t curIndex = 0;

    template <typename T>
    void read(T* ptr)
    {
        assert((curIndex + sizeof(T)) <= size);
        memcpy(ptr, &data[curIndex], sizeof(T));
        curIndex += sizeof(T);
    }
};

struct XEFVector
{
    float elements[4];
};

enum struct XEFTrackingState : int32_t
{
    NOT_TRACKED = 0,
    INFERRED,
    TRACKED
};

enum struct XEFJointType : int32_t
{
    SpineBase = 0,
    SpineMid,
    Neck,
    Head,
    ShoulderLeft,
    ElbowLeft,
    WristLeft,
    HandLeft,
    ShoulderRight,
    ElbowRight,
    WristRight,
    HandRight,
    HipLeft,
    KneeLeft,
    AnkleLeft,
    FootLeft,
    HipRight,
    KneeRight,
    AnkleRight,
    FootRight,
    SpineShoulder,
    HandTipLeft,
    ThumbLeft,
    HandTipRight,
    ThumbRight,
    SIZE
};

enum struct XEFExpressionConfidenceLevel : int32_t
{
    NONE = 0,
    LOW,
    HIGH,
    UNKNOWN
};

struct XEFExpressionData
{
    XEFExpressionConfidenceLevel Face_Neutral;
    XEFExpressionConfidenceLevel Face_Happy;

    XEFExpressionConfidenceLevel Activity_Talking;
    XEFExpressionConfidenceLevel Activity_EyeLeftClosed;
    XEFExpressionConfidenceLevel Activity_EyeRightClosed;
    XEFExpressionConfidenceLevel Activity_MouthOpen;
    XEFExpressionConfidenceLevel Activity_MouthMoved;
    XEFExpressionConfidenceLevel Activity_LookingAway;

    int32_t unknown0;
    int32_t unknown1;
    int32_t unknown2;
    int32_t unknown3;

    XEFExpressionConfidenceLevel Engagement_Engaged;
    XEFExpressionConfidenceLevel Appearance_WearingGlasses;

    int32_t unknown4;
    int32_t unknown5;
    int32_t unknown6;
    int32_t unknown7;
    int32_t unknown8;

    XEFTrackingState TrackingState;
};

enum struct XEFHandState : int32_t
{
    OPEN = 0,
    CLOSED,
    LASSO,
    UNKNOWN
};

struct XEFHandData
{
    XEFHandState HandState;
    XEFTrackingState HandTrackingState;
    XEFTrackingState HandConfidence;
};

struct XEFLeanData
{
    float X;
    float Y;

    uint32_t Reserved;

    XEFTrackingState LeanTrackingState;
};

enum struct XEFBodyTrackingState : int32_t
{
    NOT_TRACKED = 0,
    TRACKED
};

struct XEFBodyData
{
    std::array<XEFVector, int32_t(XEFJointType::SIZE)> SkeletonJointPositions;
    std::array<XEFVector, int32_t(XEFJointType::SIZE)> SkeletonJointOrientations;
    std::array<XEFTrackingState, int32_t(XEFJointType::SIZE)> SkeletonJointPositionTrackingStates;

    XEFExpressionData ExpressionData;

    XEFHandData HandDataLeft;
    XEFHandData HandDataRight;

    uint64_t TrackingID;
    XEFBodyTrackingState TrackingState;

    XEFLeanData LeanData;

    uint32_t QualityFlags;

    void fillFromByteStream(ByteStream& stream);
};

static constexpr int BODY_DATA_SIZE = 6;
struct XEFBodyFrame
{
    XEFVector FloorClipPlane;
    XEFVector Up;
    std::array<XEFBodyData, BODY_DATA_SIZE> BodyData;

    uint32_t QualityFlags;

    void fillFromByteStream(ByteStream stream);
};