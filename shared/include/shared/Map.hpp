#pragma once
#include <cstdint>

enum TileFlags : uint8_t {
    TILE_OPEN    = 0,
    TILE_BLOCKED = 1 << 0,
    TILE_WALL_N  = 1 << 1,
    TILE_WALL_E  = 1 << 2,
    TILE_WALL_S  = 1 << 3,
    TILE_WALL_W  = 1 << 4,
};

constexpr int kChunkSize = 64;
constexpr float kMaxTileHeight = 8.0f;

struct TileData {
    uint8_t flags;
    uint8_t height;
};

#pragma pack(push, 1)
struct MapHeader {
    char     magic[4] = {'O','M','A','P'};
    uint16_t width;
    uint16_t height;
};
#pragma pack(pop)
