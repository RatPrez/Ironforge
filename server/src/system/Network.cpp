#include "system/Network.hpp"

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
