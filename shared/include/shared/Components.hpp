#pragma once
#include <cstdint>

struct Actor {};

struct Position
{
    uint16_t x, y;
    uint8_t heading; // 0-7
};
