#include "Editor.hpp"
#include <cstdio>

void Export_Chunk(const EditorState& state, const char* path)
{
    MapHeader hdr;
    hdr.width  = kChunkSize;
    hdr.height = kChunkSize;
    FILE* f = fopen(path, "wb");
    if (!f) return;
    fwrite(&hdr, sizeof(hdr), 1, f);
    fwrite(state.tiles, sizeof(TileData), kChunkSize * kChunkSize, f);
    fclose(f);
}
