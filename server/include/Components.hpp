#pragma once
#include <cstdint>
#include <vector>
#include <utility>
#include <unordered_set>
#include <steam/steamnetworkingsockets.h>

struct MoveTarget
{
    uint16_t x, y;
};

struct MovePath
{
    std::vector<std::pair<uint16_t, uint16_t>> steps;
    size_t index = 0;
};

struct Player
{
    HSteamNetConnection conn;
};

struct KnownEntities
{
    std::unordered_set<uint32_t> netIds;
};

struct Running {};

struct PatrolArea
{
    int minX;
    int maxX;
    int minY;
    int maxY;

    float idleFinishTime = -1.f;
    bool hasTarget = false;
    int targetX;
    int targetY;
};

struct TestMessage
{
    float lastAttempt = 0;
    float lastMessage = 0;
};
