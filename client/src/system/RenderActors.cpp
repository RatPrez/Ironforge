#include "system/RenderActors.hpp"
#include <raylib.h>
#include <rlgl.h>

#include "base/AssetCache.hpp"

#include "Components.hpp"
#include "shared/Components.hpp"

static constexpr float kCharacterScale = 0.8f;
static constexpr int kIdleFps = 8;
static constexpr int kWalkFps = 18;
static constexpr int kRunFps  = 20;

void System::RenderActors(WorldContext& ctx, const float& dt)
{
    auto* model = ctx.assets.getModel("character");

    auto view = ctx.registry.view<RenderPosition, Actor, AnimationState>();
    for (auto entity : view) {
        auto& rp   = view.get<RenderPosition>(entity);
        auto& anim = view.get<AnimationState>(entity);

        std::string wanted = "idle";
        int fps = kIdleFps;

        if (rp.moveStartTime >= 0.f) {
            wanted = rp.isRunning ? "run" : "walk";
            fps = rp.isRunning ? kRunFps : kWalkFps;
        }

        if (wanted != anim.current) {
            anim.current = wanted;
            anim.timer = 0.f;
        }

        const ModelAnimation* clip = ctx.assets.getAnimation("character", anim.current);

        if (model && clip && clip->frameCount > 0) {
            anim.timer += dt;
            int frame = (int)(anim.timer * fps) % clip->frameCount;
            UpdateModelAnimation(*model, *clip, frame);
        }

        rlPushMatrix();
            rlTranslatef(rp.x, rp.y, rp.z);
            rlRotatef(rp.heading, 0.f, 1.f, 0.f);
            if (model) DrawModel(*model, { 0, 0, 0 }, kCharacterScale, WHITE);
        rlPopMatrix();
    }
}
