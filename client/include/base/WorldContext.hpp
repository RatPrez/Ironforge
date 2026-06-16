#pragma once
#include <entt/entt.hpp>

class AssetCache;
struct CamSettings;
struct Camera3D;
struct NetContext;

struct WorldContext {
    entt::registry& registry;
    NetContext&     net;
    Camera3D&       camera;
    AssetCache&     assets;
    CamSettings&    camSettings;
};
