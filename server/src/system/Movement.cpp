#include "system/Movement.hpp"

#include "shared/Components.hpp"
#include "shared/Packets.hpp"
#include "Components.hpp"
#include "NetBroadcast.hpp"
#include "Util.hpp"

void System::Movement(WorldContext& ctx)
{
    auto view = ctx.registry.view<MovePath, Position>();
    for (auto entity : view) {
        auto& path = view.get<MovePath>(entity);
        auto& pos  = view.get<Position>(entity);
        bool running = ctx.registry.all_of<Running>(entity);

        auto [tx, ty] = path.steps[path.index];
        pos.heading = Util::getHeadingFromTarget(pos.x, pos.y, tx, ty);

        SPacketPosition pkt;
        pkt.netId   = static_cast<uint32_t>(entity);

        if (running && path.index + 1 < path.steps.size()) {
            auto [tx2, ty2] = path.steps[path.index + 1];
            pos.x = tx2;
            pos.y = ty2;
            path.index += 2;

            pkt.isRunning = true;
            pkt.midX = tx;
            pkt.midY = ty;
        } else {
            pos.x = tx;
            pos.y = ty;
            path.index += 1;

            pkt.isRunning = false;
        }

        pkt.x       = pos.x;
        pkt.y       = pos.y;
        pkt.heading = pos.heading;
        sendToKnown(ctx, pkt.netId, pkt);

        if (path.index >= path.steps.size()) {
            ctx.registry.remove<MovePath>(entity);
        }
    }
}
