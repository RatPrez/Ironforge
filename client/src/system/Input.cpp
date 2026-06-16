#include "system/Input.hpp"

#include <raylib.h>

#include "shared/Base.hpp"
#include "shared/Packets.hpp"
#include "base/NetContext.hpp"
#include "base/DebugTerminal.hpp"

void System::Input(WorldContext& ctx)
{
    if (!IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        return;
    }

    Ray ray = GetScreenToWorldRay(GetMousePosition(), ctx.camera);
    float t = -ray.position.y / ray.direction.y;
    if (t <= 0) {
        return;
    }

    Vector3 hitPoint = {
        ray.position.x + t * ray.direction.x,
        0,
        ray.position.z + t * ray.direction.z
    };

    int gridX = (int)hitPoint.x / Base::kTileSize;
    int gridY = (int)hitPoint.z / Base::kTileSize;

    CPacketMove pkt;
    pkt.x = gridX;
    pkt.y = gridY;
    ctx.net.outbox.push(pkt);

    DebugTerminal::Instance()->log("Clicked X: " + std::to_string(gridX) + ", Y: " + std::to_string(gridY));
}
