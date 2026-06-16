#pragma once
#include <entt/entt.hpp>
#include "NetContext.hpp"

struct WorldContext {
    entt::registry& registry;
    NetContext&     net;
};
