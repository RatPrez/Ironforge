#include "base/AssetCache.hpp"
#include <raylib.h>

#include "shared/Base.hpp"

const Model* AssetCache::getModel(const std::string& modelName)
{
    if (modelName != "world") return nullptr;

    auto m = m_models.find(modelName);
    if (m == m_models.end()) {
        Mesh mesh = GenMeshPlane(
            Base::kWorldSize * Base::kTileSize,
            Base::kWorldSize * Base::kTileSize,
            Base::kWorldSize,
            Base::kWorldSize
        );

        Model model = LoadModelFromMesh(mesh);
        auto [it, _] = m_models.emplace(modelName, model);
        return &it->second;
    }

    return &m->second;
}
