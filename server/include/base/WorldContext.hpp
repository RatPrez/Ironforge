#pragma once
#include <entt/entt.hpp>
#include "NetContext.hpp"
#include "shared/Map.hpp"

struct WorldContext {
    entt::registry& registry;
    NetContext&     net;
    const TileData  (&map)[kChunkSize][kChunkSize];
    float           time; // seconds since server start, updated once per tick
};
