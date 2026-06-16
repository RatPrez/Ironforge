#pragma once
#include <string>

struct LocalPlayer {};

struct Actor {};

struct AnimationState
{
    float timer = 0.f;
    float fps   = 17.f;
    std::string current; // name of the clip currently playing
};

struct RenderPosition
{
    float x, y, z;
    float startX, startY, startZ;
    float moveStartTime = -1.f; // -1 = not moving
    float heading;        // currently rendered heading
    float startHeading;   // heading at the start of the current transition
    float targetHeading;  // heading we're rotating toward
};

struct ChatBubble
{
    std::string text;
    float expireTime;
};
