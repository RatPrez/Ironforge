#pragma once
#include <string>

struct LocalPlayer {};

struct AnimationState
{
    float timer = 0.f;
    float fps   = 0.f;
    std::string current; // name of the clip currently playing
};

struct RenderPosition
{
    float x, y, z;
    float startX, startY, startZ;
    float moveStartTime = -1.f; // -1 = not moving
    float heading;
    float startHeading;
    float targetHeading;
    bool  isRunning = false;
    float midX = 0.f, midY = 0.f, midZ = 0.f;
};

struct ChatBubble
{
    std::string text;
    float expireTime;
};
