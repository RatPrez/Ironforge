#pragma once
#include <string>
#include <unordered_map>
#include "shared/Map.hpp"
#include <raylib.h>

struct Model;

class AssetCache
{
public:
    const Model* getModel(const std::string& modelName);
    float heightAt(uint16_t tileX, uint16_t tileY) const;

private:
    std::unordered_map<std::string, Model> m_models;
    TileData m_map[kChunkSize][kChunkSize] = {};
    bool     m_mapLoaded = false;
    Shader   m_terrainShader = {};
    bool     m_shaderLoaded = false;
    void ensureMapLoaded();
    Shader getTerrainShader();
};
