#include "Editor.hpp"
#include <raylib.h>
#include <raymath.h>
#include <cmath>

static Vector3 raycastAtY(const Ray& ray, float planeY)
{
    if (fabsf(ray.direction.y) < 1e-6f) return { -1.f, 0.f, -1.f };
    float t = (planeY - ray.position.y) / ray.direction.y;
    if (t <= 0.f) return { -1.f, 0.f, -1.f };
    return { ray.position.x + t * ray.direction.x, planeY, ray.position.z + t * ray.direction.z };
}

static Vector3 raycastFlat(const Camera3D& cam, const EditorState& state)
{
    Ray ray = GetScreenToWorldRay(GetMousePosition(), cam);

    // first pass at Y=0 to estimate tile
    Vector3 hit = raycastAtY(ray, 0.f);
    if (hit.x < 0.f) return hit;

    // refine using that tile's actual height
    int gx = (int)(hit.x / kEditorTileSize);
    int gy = (int)(hit.z / kEditorTileSize);
    if (gx >= 0 && gx < kChunkSize && gy >= 0 && gy < kChunkSize) {
        float surfaceY = state.tiles[gy][gx].height / 255.f * kMaxTileHeight;
        hit = raycastAtY(ray, surfaceY);
    }
    return hit;
}

static void orbitCamera(EditorState& state)
{
    static float yaw    = 45.f;
    static float pitch  = 45.f;
    static float radius = 80.f;
    static float panX   = kChunkSize * kEditorTileSize * 0.5f;
    static float panZ   = kChunkSize * kEditorTileSize * 0.5f;

    if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
        Vector2 delta = GetMouseDelta();
        yaw   += delta.x * 0.3f;
        pitch  = Clamp(pitch + delta.y * 0.3f, 5.f, 89.f);
    }

    float wheel = GetMouseWheelMove();
    radius = Clamp(radius - wheel * 4.f, 10.f, 200.f);

    float dt     = GetFrameTime();
    float speed  = radius * 0.8f * dt;
    float yawR   = DEG2RAD * yaw;
    // forward/right relative to camera yaw (flat plane)
    float fwdX =  sinf(yawR);
    float fwdZ =  cosf(yawR);
    float rgtX =  cosf(yawR);
    float rgtZ = -sinf(yawR);

    if (IsKeyDown(KEY_W)) { panX -= fwdX * speed; panZ -= fwdZ * speed; }
    if (IsKeyDown(KEY_S)) { panX += fwdX * speed; panZ += fwdZ * speed; }
    if (IsKeyDown(KEY_A)) { panX -= rgtX * speed; panZ -= rgtZ * speed; }
    if (IsKeyDown(KEY_D)) { panX += rgtX * speed; panZ += rgtZ * speed; }

    float pitchR = DEG2RAD * pitch;
    float cx = radius * cosf(pitchR) * sinf(yawR);
    float cy = radius * sinf(pitchR);
    float cz = radius * cosf(pitchR) * cosf(yawR);

    state.camera.position = { panX + cx, cy, panZ + cz };
    state.camera.target   = { panX, 0.f, panZ };
    state.camera.up       = { 0.f, 1.f, 0.f };
    state.camera.fovy     = 60.f;
    state.camera.projection = CAMERA_PERSPECTIVE;
}

void Editor_Update(EditorState& state)
{
    orbitCamera(state);

    // tool hotkeys
    if (IsKeyPressed(KEY_ONE))   state.activeTool = TOOL_RAISE;
    if (IsKeyPressed(KEY_TWO))   state.activeTool = TOOL_LOWER;
    if (IsKeyPressed(KEY_THREE)) state.activeTool = TOOL_BLOCK;
    if (IsKeyPressed(KEY_FOUR))  state.activeTool = TOOL_WALL;

    if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_S)) {
        Export_Chunk(state, "chunk_0_0.omap");
        TraceLog(LOG_INFO, "Exported chunk_0_0.omap");
    }

    Vector3 hit = raycastFlat(state.camera, state);
    int gx = (int)(hit.x / kEditorTileSize);
    int gy = (int)(hit.z / kEditorTileSize);
    bool inBounds = (gx >= 0 && gx < kChunkSize && gy >= 0 && gy < kChunkSize);
    state.hoveredX = inBounds ? gx : -1;
    state.hoveredY = inBounds ? gy : -1;

    if (!inBounds || IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) return;

    bool clicked = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
    bool held    = IsMouseButtonDown(MOUSE_BUTTON_LEFT);

    TileData& tile = state.tiles[gy][gx];

    if (state.activeTool == TOOL_RAISE && held) {
        if (tile.height < 255) { tile.height++; Grid_UpdateMesh(state); }
    } else if (state.activeTool == TOOL_LOWER && held) {
        if (tile.height > 0)   { tile.height--; Grid_UpdateMesh(state); }
    } else if (state.activeTool == TOOL_BLOCK && clicked) {
        tile.flags ^= TILE_BLOCKED;
    } else if (state.activeTool == TOOL_WALL && clicked) {
        // determine closest edge from fractional position
        float fx = (hit.x / kEditorTileSize) - gx;
        float fz = (hit.z / kEditorTileSize) - gy;
        uint8_t wall = 0;
        if (fz < 0.25f)       wall = TILE_WALL_N;
        else if (fz > 0.75f)  wall = TILE_WALL_S;
        else if (fx < 0.25f)  wall = TILE_WALL_W;
        else if (fx > 0.75f)  wall = TILE_WALL_E;
        if (wall) tile.flags ^= wall;
    }
}

void Editor_DrawOverlays(const EditorState& state)
{
    for (int y = 0; y < kChunkSize; y++) {
        for (int x = 0; x < kChunkSize; x++) {
            const TileData& tile = state.tiles[y][x];
            float cx = x * kEditorTileSize + kEditorTileSize * 0.5f;
            float cz = y * kEditorTileSize + kEditorTileSize * 0.5f;
            float cy = tile.height / 255.f * kMaxTileHeight + 0.05f;

            if (tile.flags & TILE_BLOCKED) {
                DrawCube({ cx, cy, cz }, kEditorTileSize, 0.05f, kEditorTileSize, { 255, 0, 0, 100 });
            }

            auto drawWall = [&](uint8_t flag, Vector3 pos, float w, float d) {
                if (tile.flags & flag)
                    DrawCube(pos, w, 0.5f, d, { 255, 140, 0, 200 });
            };
            float h = 0.25f;
            drawWall(TILE_WALL_N, { cx, cy + h, cz - kEditorTileSize * 0.5f }, kEditorTileSize, 0.1f);
            drawWall(TILE_WALL_S, { cx, cy + h, cz + kEditorTileSize * 0.5f }, kEditorTileSize, 0.1f);
            drawWall(TILE_WALL_W, { cx - kEditorTileSize * 0.5f, cy + h, cz }, 0.1f, kEditorTileSize);
            drawWall(TILE_WALL_E, { cx + kEditorTileSize * 0.5f, cy + h, cz }, 0.1f, kEditorTileSize);

            if (x == state.hoveredX && y == state.hoveredY &&
                state.activeTool != TOOL_RAISE && state.activeTool != TOOL_LOWER) {
                DrawCube({ cx, cy, cz }, kEditorTileSize, 0.05f, kEditorTileSize, { 255, 255, 0, 80 });
            }
        }
    }

    // vertex sphere for raise/lower — snaps to the nearest grid corner
    if ((state.activeTool == TOOL_RAISE || state.activeTool == TOOL_LOWER) &&
        state.hoveredX >= 0 && state.hoveredY >= 0)
    {
        int vx = state.hoveredX;
        int vy = state.hoveredY;
        // clamp so we always index a valid tile for its height
        int tx = (vx < kChunkSize) ? vx : kChunkSize - 1;
        int ty = (vy < kChunkSize) ? vy : kChunkSize - 1;
        float vy_world = state.tiles[ty][tx].height / 255.f * kMaxTileHeight;
        DrawSphere({ vx * kEditorTileSize, vy_world, vy * kEditorTileSize }, 0.3f, YELLOW);
    }
}
