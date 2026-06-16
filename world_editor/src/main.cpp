#include "Editor.hpp"
#include <raylib.h>

static const char* toolName(Tool t)
{
    switch (t) {
    case TOOL_RAISE: return "1: Raise";
    case TOOL_LOWER: return "2: Lower";
    case TOOL_BLOCK: return "3: Block";
    case TOOL_WALL:  return "4: Wall";
    }
    return "";
}

int main()
{
    InitWindow(1280, 720, "World Editor");
    SetTargetFPS(60);

    EditorState state;
    // start at mid-height so raise and lower both work from the start
    for (int y = 0; y < kChunkSize; y++)
        for (int x = 0; x < kChunkSize; x++)
            state.tiles[y][x].height = 128;
    Grid_Build(state);

    while (!WindowShouldClose()) {
        Editor_Update(state);

        BeginDrawing();
        ClearBackground(DARKGRAY);

        BeginMode3D(state.camera);
            DrawModel(state.model, { 0.f, 0.f, 0.f }, 1.f, LIGHTGRAY);
            DrawModelWires(state.model, { 0.f, 0.f, 0.f }, 1.f, { 80, 80, 80, 180 });
            Editor_DrawOverlays(state);
        EndMode3D();

        DrawText(toolName(state.activeTool), 10, 10, 20, WHITE);
        DrawText("RMB: orbit  Scroll: zoom  Ctrl+S: export", 10, 35, 16, LIGHTGRAY);
        if (state.hoveredX >= 0)
            DrawText(TextFormat("Tile (%d, %d)", state.hoveredX, state.hoveredY), 10, 55, 16, YELLOW);

        EndDrawing();
    }

    UnloadModel(state.model);
    CloseWindow();
    return 0;
}
