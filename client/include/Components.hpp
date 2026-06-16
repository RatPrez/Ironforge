#pragma once

struct LocalPlayer {};

struct Actor {};

struct RenderPosition
{
    float x, y, z;
    float startX, startY, startZ;
    float moveStartTime = -1.f; // -1 = not moving
    float heading;
};
