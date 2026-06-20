#include "system/MovementInterp.hpp"
#include <raylib.h>
#include <raymath.h>
#include <cmath>

#include "Base.hpp"
#include "shared/Components.hpp"
#include "base/AssetCache.hpp"
#include "Components.hpp"

static constexpr float kTickDuration    = 0.6f;
static constexpr float kHalfTick        = kTickDuration * 0.5f;
static constexpr float kHeadingDuration = 0.3f;
static constexpr float kTileCenterOffset = Base::kTileSize * 0.5f;

// shortest-path angle interpolation, e.g. 315 -> 0 rotates +45, not -315
static float LerpAngle(float from, float to, float t)
{
    float diff = fmodf(to - from + 540.f, 360.f) - 180.f;
    return from + diff * t;
}

void System::MovementInterp(WorldContext& ctx, const float& dt)
{
    auto view = ctx.registry.view<Position, RenderPosition>();
    for (auto entity : view) {
        auto& pos  = view.get<Position>(entity);
        auto& rpos = view.get<RenderPosition>(entity);

        if (rpos.moveStartTime < 0.f) continue;

        float targetX = pos.x * (float)Base::kTileSize + kTileCenterOffset;
        float targetZ = pos.y * (float)Base::kTileSize + kTileCenterOffset;
        float targetY = ctx.assets.heightAt(pos.x, pos.y);

        float elapsed = (float)GetTime() - rpos.moveStartTime;
        float headingProgress = Clamp(elapsed / kHeadingDuration, 0.f, 1.f);
        rpos.heading = LerpAngle(rpos.startHeading, rpos.targetHeading, headingProgress);

        if (rpos.isRunning) {
            if (elapsed < kHalfTick) {
                float t = Clamp(elapsed / kHalfTick, 0.f, 1.f);
                rpos.x = Lerp(rpos.startX, rpos.midX, t);
                rpos.y = Lerp(rpos.startY, rpos.midY, t);
                rpos.z = Lerp(rpos.startZ, rpos.midZ, t);
            } else {
                float t = Clamp((elapsed - kHalfTick) / kHalfTick, 0.f, 1.f);
                rpos.x = Lerp(rpos.midX, targetX, t);
                rpos.y = Lerp(rpos.midY, targetY, t);
                rpos.z = Lerp(rpos.midZ, targetZ, t);
            }
            if (elapsed >= kTickDuration) rpos.moveStartTime = -1.f;
        } else {
            float progress = Clamp(elapsed / kTickDuration, 0.f, 1.f);
            rpos.x = Lerp(rpos.startX, targetX, progress);
            rpos.y = Lerp(rpos.startY, targetY, progress);
            rpos.z = Lerp(rpos.startZ, targetZ, progress);
            if (progress >= 1.f) rpos.moveStartTime = -1.f;
        }
    }
}
