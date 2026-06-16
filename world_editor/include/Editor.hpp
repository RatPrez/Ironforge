#pragma once
#include "shared/Map.hpp"
#include <raylib.h>

enum Tool { TOOL_RAISE, TOOL_LOWER, TOOL_BLOCK, TOOL_WALL };

constexpr float kEditorTileSize = 2.0f;

struct EditorState {
    TileData tiles[kChunkSize][kChunkSize] = {};
    Tool     activeTool = TOOL_RAISE;
    Camera3D camera     = {};
    Mesh     mesh       = {};
    Model    model      = {};
    float*   vertices   = nullptr; // owned copy for UpdateMeshBuffer
    int      hoveredX   = -1;
    int      hoveredY   = -1;
};

void Grid_Build(EditorState& state);
void Grid_UpdateMesh(EditorState& state);
void Editor_Update(EditorState& state);
void Editor_DrawOverlays(const EditorState& state);
void Export_Chunk(const EditorState& state, const char* path);
