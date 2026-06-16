#include "system/RenderWorld.hpp"
#include <raylib.h>

#include "base/AssetCache.hpp"

void System::RenderWorld(WorldContext& ctx)
{
    auto* model = ctx.assets.getModel("world");

    DrawModel(*model, { 0.0f, 0.0f, 0.0f }, 1.0f, Color{ 130, 128, 122, 255 });
}
