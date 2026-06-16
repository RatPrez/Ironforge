#include "base/AssetCache.hpp"
#include <raylib.h>
#include <raymath.h>
#include <cstdlib>
#include <cstring>

#include "shared/Base.hpp"
#include "shared/Map.hpp"
#include "shared/MapLoader.hpp"

static constexpr int kVerts      = (kChunkSize + 1) * (kChunkSize + 1);
static constexpr int kIndexCount = kChunkSize * kChunkSize * 6;

void AssetCache::ensureMapLoaded()
{
    if (m_mapLoaded) return;
    memset(m_map, 0, sizeof(m_map));
    loadMap("chunk_0_0.omap", m_map);
    m_mapLoaded = true;
}

Shader AssetCache::getTerrainShader()
{
    if (!m_shaderLoaded) {
        m_terrainShader = LoadShader(
            "client/assets/shaders/terrain.vs",
            "client/assets/shaders/terrain.fs");
        int loc = GetShaderLocation(m_terrainShader, "tileSize");
        float tileSize = (float)Base::kTileSize;
        SetShaderValue(m_terrainShader, loc, &tileSize, SHADER_UNIFORM_FLOAT);
        m_shaderLoaded = true;
    }
    return m_terrainShader;
}

float AssetCache::heightAt(uint16_t tileX, uint16_t tileY) const
{
    if (tileX >= kChunkSize || tileY >= kChunkSize) return 0.f;
    return m_map[tileY][tileX].height / 255.f * kMaxTileHeight;
}

static float sampleHeight(const TileData (&map)[kChunkSize][kChunkSize], int col, int row)
{
    int tc = (col < 0) ? 0 : (col >= kChunkSize ? kChunkSize - 1 : col);
    int tr = (row < 0) ? 0 : (row >= kChunkSize ? kChunkSize - 1 : row);
    return map[tr][tc].height / 255.f * kMaxTileHeight;
}

static Model buildWorldMesh(const TileData (&map)[kChunkSize][kChunkSize])
{
    float* verts   = (float*)malloc(kVerts * 3 * sizeof(float));
    float* normals = (float*)malloc(kVerts * 3 * sizeof(float));
    unsigned short* indices = (unsigned short*)malloc(kIndexCount * sizeof(unsigned short));

    for (int row = 0; row <= kChunkSize; row++) {
        for (int col = 0; col <= kChunkSize; col++) {
            float y = sampleHeight(map, col, row);
            int vi = (row * (kChunkSize + 1) + col) * 3;
            verts[vi + 0] = col * Base::kTileSize;
            verts[vi + 1] = y;
            verts[vi + 2] = row * Base::kTileSize;

            float hL = sampleHeight(map, col - 1, row);
            float hR = sampleHeight(map, col + 1, row);
            float hU = sampleHeight(map, col, row - 1);
            float hD = sampleHeight(map, col, row + 1);
            float dx = (hR - hL) / (2.f * Base::kTileSize);
            float dz = (hD - hU) / (2.f * Base::kTileSize);
            Vector3 n = Vector3Normalize({ -dx, 1.f, -dz });
            normals[vi + 0] = n.x;
            normals[vi + 1] = n.y;
            normals[vi + 2] = n.z;
        }
    }

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

    Mesh mesh = {};
    mesh.vertexCount   = kVerts;
    mesh.triangleCount = kIndexCount / 3;
    mesh.vertices      = verts;
    mesh.normals       = normals;
    mesh.indices       = indices;
    UploadMesh(&mesh, false);

    return LoadModelFromMesh(mesh);
}

const Model* AssetCache::getModel(const std::string& modelName)
{
    auto it = m_models.find(modelName);
    if (it != m_models.end()) return &it->second;

    if (modelName != "world") return nullptr;

    ensureMapLoaded();
    Model model = buildWorldMesh(m_map);
    model.materials[0].shader = getTerrainShader();

    auto [ins, _] = m_models.emplace(modelName, model);
    return &ins->second;
}
