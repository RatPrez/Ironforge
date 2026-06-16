#include "system/Sync.hpp"

#include <unordered_set>

#include "shared/Components.hpp"
#include "shared/Packets.hpp"
#include "Components.hpp"

void System::Sync(WorldContext& ctx)
{
    auto players = ctx.registry.view<Player, KnownEntities>();
    auto world   = ctx.registry.view<Position>();

    // build current world entity set once, reused per player
    std::unordered_set<uint32_t> visible;
    for (auto ent : world)
        visible.insert(static_cast<uint32_t>(ent));

    for (auto playerEnt : players) {
        auto& player = players.get<Player>(playerEnt);
        auto& known  = players.get<KnownEntities>(playerEnt);

        // spawn entities the player doesn't know about yet
        for (uint32_t netId : visible) {
            if (known.netIds.count(netId)) continue;
            auto& pos = ctx.registry.get<Position>(static_cast<entt::entity>(netId));
            SPacketSpawn pkt;
            pkt.netId   = netId;
            pkt.x       = pos.x;
            pkt.y       = pos.y;
            pkt.heading = pos.heading;
            ctx.net.outbox.send(player.conn, pkt);
            known.netIds.insert(netId);
        }

        // despawn entities that no longer exist
        for (auto it = known.netIds.begin(); it != known.netIds.end(); ) {
            if (!visible.count(*it)) {
                SPacketDespawn pkt;
                pkt.netId = *it;
                ctx.net.outbox.send(player.conn, pkt);
                it = known.netIds.erase(it);
            } else {
                ++it;
            }
        }
    }
}
