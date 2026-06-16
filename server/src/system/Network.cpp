#include "system/Network.hpp"

#include <cstring>

#include "base/WorldContext.hpp"
#include "base/NetContext.hpp"
#include "shared/Packets.hpp"
#include "shared/Components.hpp"
#include "Components.hpp"

namespace
{ // handlers
    void handleMove(WorldContext& ctx, HSteamNetConnection conn, void* pData)
    {
        auto it = ctx.net.connToEntity.find(conn);
        if (it == ctx.net.connToEntity.end()) return;
        entt::entity entity = it->second;
        auto* pkt = reinterpret_cast<const CPacketMove*>(pData);
        ctx.registry.emplace_or_replace<MoveTarget>(entity, (uint16_t)pkt->x, (uint16_t)pkt->y);
    }

    void handleChat(WorldContext& ctx, HSteamNetConnection conn, void* pData)
    {
        auto it = ctx.net.connToEntity.find(conn);
        if (it == ctx.net.connToEntity.end()) return;
        auto* pkt = reinterpret_cast<const CPacketChat*>(pData);

        SPacketChat out;
        out.netId = static_cast<uint32_t>(it->second);
        memcpy(out.text, pkt->text, sizeof(out.text));
        out.text[sizeof(out.text) - 1] = '\0';

        auto players = ctx.registry.view<Player, KnownEntities>();
        for (auto playerEnt : players) {
            auto& known = players.get<KnownEntities>(playerEnt);
            if (!known.netIds.count(out.netId)) continue;
            ctx.net.outbox.send(players.get<Player>(playerEnt).conn, out);
        }
    }
}

void System::NetReceive(WorldContext& ctx)
{
    ctx.net.sockets->RunCallbacks();

    ISteamNetworkingMessage* msgs[64];
    int count = ctx.net.sockets->ReceiveMessagesOnPollGroup(ctx.net.pollGroup, msgs, 64);
    for (int i = 0; i < count; i++) {
        ISteamNetworkingMessage* msg = msgs[i];
        if (msg->m_cbSize < (int)sizeof(PacketHeader)) {
            msg->Release();
            continue;
        }
        auto* hdr = reinterpret_cast<const PacketHeader*>(msg->m_pData);
        switch (hdr->type) {
        case PacketType::CMove:
            handleMove(ctx, msg->m_conn, msg->m_pData);
            break;
        case PacketType::CChat:
            handleChat(ctx, msg->m_conn, msg->m_pData);
            break;
        }
        msg->Release();
    }
}

void System::NetSend(WorldContext& ctx)
{
    for (auto& [conn, queue] : ctx.net.outbox.packets) {
        for (auto& packet : queue) {
            ctx.net.sockets->SendMessageToConnection(
                conn,
                packet.data(),
                (uint32_t)packet.size(),
                k_nSteamNetworkingSend_Reliable,
                nullptr
            );
        }
    }
    ctx.net.outbox.packets.clear();
}
