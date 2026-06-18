#include "system/RenderActors.hpp"
#include <raylib.h>
#include <rlgl.h>

#include "base/AssetCache.hpp"
#include "Components.hpp"

static constexpr float kCharacterScale = 0.8f;
static constexpr float kIdleFps       = 7.f;
static constexpr float kWalkFps       = 14.f;

void System::RenderActors(WorldContext& ctx, const float& dt)
{
    auto* model = ctx.assets.getModel("character");

    auto view = ctx.registry.view<RenderPosition, Actor, AnimationState>();
    for (auto entity : view) {
        auto& rp   = view.get<RenderPosition>(entity);
        auto& anim = view.get<AnimationState>(entity);

        const std::string& wanted = (rp.moveStartTime >= 0.f) ? "walk" : "idle";
        if (wanted != anim.current) {
            anim.fps = wanted == "idle" ? kIdleFps : kWalkFps;
            anim.current = wanted;
            anim.timer = 0.f;
        }

        const ModelAnimation* clip = ctx.assets.getAnimation("character", anim.current);

        if (model && clip && clip->frameCount > 0) {
            anim.timer += dt;
            int frame = (int)(anim.timer * anim.fps) % clip->frameCount;
            UpdateModelAnimation(*model, *clip, frame);
        }

        rlPushMatrix();
            rlTranslatef(rp.x, rp.y, rp.z);
            rlRotatef(rp.heading, 0.f, 1.f, 0.f);
            if (model) DrawModel(*model, { 0, 0, 0 }, kCharacterScale, WHITE);
        rlPopMatrix();
    }
}
