#pragma once
#include <cstdint>

namespace Base
{
    constexpr int      kMaxPathDistance    = 100;
    constexpr int      kServerPort         = 27020;
    constexpr int      kTickDurationMs     = 600;
    constexpr uint16_t kDefaultSpawnX      = 20;
    constexpr uint16_t kDefaultSpawnY      = 20;
    constexpr int      kMaxMessagesPerTick = 64;
    constexpr const char* kDefaultMapPath  = "chunk_0_0.omap";
}
