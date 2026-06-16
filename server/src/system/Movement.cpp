#include "system/Movement.hpp"

#include "shared/Components.hpp"
#include "shared/Packets.hpp"
#include "Components.hpp"

void System::Movement(WorldContext& ctx)
{
    auto view = ctx.registry.view<MovePath, Position>();
    for (auto entity : view) {
        auto& path = view.get<MovePath>(entity);
        auto& pos  = view.get<Position>(entity);

        auto [tx, ty] = path.steps[path.index];

        int dx = (int)tx - pos.x;
        int dy = (int)ty - pos.y;
        if      (dx ==  0 && dy ==  1) pos.heading = 0;
        else if (dx ==  1 && dy ==  1) pos.heading = 1;
        else if (dx ==  1 && dy ==  0) pos.heading = 2;
        else if (dx ==  1 && dy == -1) pos.heading = 3;
        else if (dx ==  0 && dy == -1) pos.heading = 4;
        else if (dx == -1 && dy == -1) pos.heading = 5;
        else if (dx == -1 && dy ==  0) pos.heading = 6;
        else if (dx == -1 && dy ==  1) pos.heading = 7;

        pos.x = tx;
        pos.y = ty;
        path.index++;

        SPacketPosition pkt;
        pkt.netId   = static_cast<uint32_t>(entity);
        pkt.x       = pos.x;
        pkt.y       = pos.y;
        pkt.heading = pos.heading;
        ctx.net.outbox.broadcast(ctx.net.clients, pkt);

        if (path.index >= path.steps.size()) {
            ctx.registry.remove<MovePath>(entity);
        }
    }
}
