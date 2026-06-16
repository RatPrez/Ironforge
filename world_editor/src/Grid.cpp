#include "Editor.hpp"
#include <cstdlib>
#include <cstring>

// Grid is kChunkSize x kChunkSize quads, each quad = 2 triangles = 6 indices, 4 verts.
// We use indexed mesh: (kChunkSize+1)^2 verts, kChunkSize^2 * 6 indices.

static constexpr int kVerts   = (kChunkSize + 1) * (kChunkSize + 1);
static constexpr int kIndices = kChunkSize * kChunkSize * 6;

static float tileY(const EditorState& state, int x, int y)
{
    x = (x < 0) ? 0 : (x >= kChunkSize ? kChunkSize - 1 : x);
    y = (y < 0) ? 0 : (y >= kChunkSize ? kChunkSize - 1 : y);
    return state.tiles[y][x].height / 255.f * kMaxTileHeight;
}

void Grid_Build(EditorState& state)
{
    state.vertices = (float*)malloc(kVerts * 3 * sizeof(float));

    // fill vertices
    for (int row = 0; row <= kChunkSize; row++) {
        for (int col = 0; col <= kChunkSize; col++) {
            int idx = (row * (kChunkSize + 1) + col) * 3;
            state.vertices[idx + 0] = col * kEditorTileSize;
            state.vertices[idx + 1] = tileY(state, col, row);
            state.vertices[idx + 2] = row * kEditorTileSize;
        }
    }

    unsigned short* indices = (unsigned short*)malloc(kIndices * sizeof(unsigned short));
    int ii = 0;
    for (int row = 0; row < kChunkSize; row++) {
        for (int col = 0; col < kChunkSize; col++) {
            unsigned short tl = row * (kChunkSize + 1) + col;
            unsigned short tr = tl + 1;
            unsigned short bl = tl + (kChunkSize + 1);
            unsigned short br = bl + 1;
            indices[ii++] = tl; indices[ii++] = bl; indices[ii++] = tr;
            indices[ii++] = tr; indices[ii++] = bl; indices[ii++] = br;
        }
    }

    float* normals = (float*)calloc(kVerts * 3, sizeof(float));
    for (int i = 0; i < kVerts; i++) normals[i * 3 + 1] = 1.f; // all up

    Mesh& m = state.mesh;
    m.vertexCount  = kVerts;
    m.triangleCount = kIndices / 3;
    m.vertices  = state.vertices;
    m.indices   = indices;
    m.normals   = normals;
    UploadMesh(&m, true); // dynamic = true so we can update later

    state.model = LoadModelFromMesh(state.mesh);
}

void Grid_UpdateMesh(EditorState& state)
{
    for (int row = 0; row <= kChunkSize; row++) {
        for (int col = 0; col <= kChunkSize; col++) {
            int idx = (row * (kChunkSize + 1) + col) * 3;
            state.vertices[idx + 1] = tileY(state, col, row);
        }
    }
    UpdateMeshBuffer(state.mesh, 0, state.vertices, kVerts * 3 * sizeof(float), 0);
}
