#pragma once
#include "base/WorldContext.hpp"
#include "Components.hpp"

template<typename T>
void sendToKnown(WorldContext& ctx, uint32_t netId, const T& pkt)
{
    auto players = ctx.registry.view<Player, KnownEntities>();
    for (auto playerEnt : players) {
        auto& known = players.get<KnownEntities>(playerEnt);
        if (!known.netIds.count(netId)) continue;
        ctx.net.outbox.send(players.get<Player>(playerEnt).conn, pkt);
    }
}
