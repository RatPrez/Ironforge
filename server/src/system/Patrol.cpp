#include "system/Patrol.hpp"

#include "shared/Components.hpp"
#include "Components.hpp"
#include "Util.hpp"

constexpr float kInvalidIdleTime = -1.f;
constexpr float kMinIdleTime = 3.f;
constexpr float kMaxIdleTime = 10.f;

void System::Patrol(WorldContext &ctx)
{
    float currentTime = ctx.time;
    auto view = ctx.registry.view<Position, PatrolArea>();
    for (auto entity : view) {
        auto& pos = view.get<Position>(entity);
        auto& pat = view.get<PatrolArea>(entity);

        if (pat.hasTarget && (pat.targetX != pos.x || pat.targetY != pos.y))
            continue;

        if (pat.hasTarget && pat.idleFinishTime == kInvalidIdleTime) {
            pat.idleFinishTime = currentTime + Util::getRandomValue<float>(kMinIdleTime, kMaxIdleTime);
            continue;
        }

        if (pat.hasTarget && currentTime < pat.idleFinishTime)
            continue;

        pat.idleFinishTime = kInvalidIdleTime;

        int tx = Util::getRandomValue<int>(pat.minX, pat.maxX);
        int ty = Util::getRandomValue<int>(pat.minY, pat.maxY);
        pat.targetX = tx;
        pat.targetY = ty;
        pat.hasTarget = true;
        ctx.registry.emplace_or_replace<MoveTarget>(entity, (uint16_t)tx, (uint16_t)ty);
    }
}
