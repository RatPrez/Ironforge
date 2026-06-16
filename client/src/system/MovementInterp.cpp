#include "system/MovementInterp.hpp"
#include <raylib.h>
#include <raymath.h>

#include "shared/Base.hpp"
#include "shared/Components.hpp"
#include "Components.hpp"

static constexpr float kTickDuration = 0.6f;

void System::MovementInterp(WorldContext& ctx, const float& dt)
{
    auto view = ctx.registry.view<Position, RenderPosition>();
    for (auto entity : view) {
        auto& pos  = view.get<Position>(entity);
        auto& rpos = view.get<RenderPosition>(entity);

        if (rpos.moveStartTime < 0.f) continue;

        float half = Base::kTileSize * 0.5f;
        float targetX = pos.x * (float)Base::kTileSize + half;
        float targetZ = pos.y * (float)Base::kTileSize + half;

        float progress = ((float)GetTime() - rpos.moveStartTime) / kTickDuration;
        progress = Clamp(progress, 0.f, 1.f);

        rpos.x = Lerp(rpos.startX, targetX, progress);
        rpos.z = Lerp(rpos.startZ, targetZ, progress);

        if (progress >= 1.f) {
            rpos.moveStartTime = -1.f;
        }
    }
}
