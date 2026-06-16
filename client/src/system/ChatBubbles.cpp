#include "system/ChatBubbles.hpp"
#include <raylib.h>
#include <vector>

#include "Components.hpp"

void System::ChatBubbles(WorldContext& ctx)
{
    std::vector<entt::entity> expired;

    auto view = ctx.registry.view<RenderPosition, ChatBubble>();
    for (auto entity : view) {
        auto& rp     = view.get<RenderPosition>(entity);
        auto& bubble = view.get<ChatBubble>(entity);

        if ((float)GetTime() >= bubble.expireTime) {
            expired.push_back(entity);
            continue;
        }

        Vector3 worldPos = { rp.x, rp.y + 5.0f, rp.z };
        Vector2 screenPos = GetWorldToScreen(worldPos, ctx.camera);

        int fontSize = 18;
        int textWidth = MeasureText(bubble.text.c_str(), fontSize);
        DrawText(bubble.text.c_str(), (int)screenPos.x - textWidth / 2, (int)screenPos.y, fontSize, YELLOW);
    }

    for (auto entity : expired) ctx.registry.remove<ChatBubble>(entity);
}
