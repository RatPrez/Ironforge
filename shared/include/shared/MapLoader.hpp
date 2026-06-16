#pragma once
#include "shared/Map.hpp"
#include <cstdio>
#include <cstring>

inline bool loadMap(const char* path, TileData out[kChunkSize][kChunkSize])
{
    FILE* f = fopen(path, "rb");
    if (!f) return false;
    MapHeader hdr;
    if (fread(&hdr, sizeof(hdr), 1, f) != 1 ||
        hdr.magic[0] != 'O' || hdr.magic[1] != 'M' ||
        hdr.magic[2] != 'A' || hdr.magic[3] != 'P')
    {
        fclose(f);
        return false;
    }
    fread(out, sizeof(TileData), kChunkSize * kChunkSize, f);
    fclose(f);
    return true;
}
