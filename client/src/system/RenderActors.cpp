#include "system/RenderActors.hpp"
#include <raylib.h>
#include <rlgl.h>

#include "shared/Base.hpp"
#include "Components.hpp"

void System::RenderActors(WorldContext& ctx)
{
    auto view = ctx.registry.view<RenderPosition, Actor>();
    for (auto entity : view) {
        auto& rp = view.get<RenderPosition>(entity);
        rlPushMatrix();
            rlTranslatef(rp.x, rp.y, rp.z);
            rlRotatef(rp.heading, 0.f, 1.f, 0.f);
            DrawCube({ 0, 0, 0 }, (float)Base::kTileSize, (float)Base::kTileSize * 2, (float)Base::kTileSize, RED);
        rlPopMatrix();
    }
}
