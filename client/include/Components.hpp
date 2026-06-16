#pragma once

struct LocalPlayer {};

struct Actor {
    bool state = true; // temp to init
}; // no args for an Actor for now, just a box

struct RenderPosition
{
    float x, y, z;
    float startX, startZ;
    float moveStartTime = -1.f; // -1 = not moving
    float heading;
};
