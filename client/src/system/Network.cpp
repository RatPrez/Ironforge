#include "system/Network.hpp"

#include "base/NetContext.hpp"
#include "shared/Packets.hpp"
#include "shared/Components.hpp"
#include "Components.hpp"

#include "shared/Base.hpp"
#include "base/DebugTerminal.hpp"
#include "base/AssetCache.hpp"
#include <raylib.h>
#include <cstring>

namespace
{ // handlers
    entt::entity getOrCreate(WorldContext& ctx, uint32_t netId)
    {
        auto it = ctx.net.netToEnt.find(netId);
        if (it != ctx.net.netToEnt.end()) return it->second;
        entt::entity entity = ctx.registry.create();
        ctx.net.netToEnt[netId] = entity;
        return entity;
    }

    void handleSpawn(WorldContext& ctx, void* pData)
    {
        auto* pkt = reinterpret_cast<const SPacketSpawn*>(pData);
        entt::entity entity = getOrCreate(ctx, pkt->netId);

        float half = Base::kTileSize * 0.5f;
        float wx = pkt->x * (float)Base::kTileSize + half;
        float wz = pkt->y * (float)Base::kTileSize + half;
        float wy = ctx.assets.heightAt(pkt->x, pkt->y);
        ctx.registry.emplace_or_replace<Actor>(entity);
        ctx.registry.emplace_or_replace<AnimationState>(entity);
        ctx.registry.emplace_or_replace<Position>(entity, pkt->x, pkt->y, pkt->heading);
        float heading = pkt->heading * 45.f;
        ctx.registry.emplace_or_replace<RenderPosition>(entity, wx, wy, wz, wx, wy, wz, -1.f, heading, heading, heading);

        DebugTerminal::Instance()->log("Entity spawned: " + std::to_string(pkt->netId));
    }

    void handleSelf(WorldContext& ctx, void* pData)
    {
        auto* pkt = reinterpret_cast<const SPacketSelf*>(pData);
        entt::entity entity = getOrCreate(ctx, pkt->netId);

        ctx.registry.emplace_or_replace<LocalPlayer>(entity);
        DebugTerminal::Instance()->log("Local player set: " + std::to_string(pkt->netId));
    }

    void handleDespawn(WorldContext& ctx, void* pData)
    {
        auto* pkt = reinterpret_cast<const SPacketDespawn*>(pData);
        auto it = ctx.net.netToEnt.find(pkt->netId);
        if (it == ctx.net.netToEnt.end()) return;
        ctx.registry.destroy(it->second);
        ctx.net.netToEnt.erase(it);
        DebugTerminal::Instance()->log("Entity despawned: " + std::to_string(pkt->netId));
    }

    void handlePosition(WorldContext& ctx, void* pData)
    {
        auto* pkt = reinterpret_cast<const SPacketPosition*>(pData);

        auto it = ctx.net.netToEnt.find(pkt->netId);
        if (it == ctx.net.netToEnt.end()) {
            return;
        }

        entt::entity entity = it->second;
        ctx.registry.patch<Position>(entity, [&](Position& pos) {
            pos.x       = pkt->x;
            pos.y       = pkt->y;
            pos.heading = pkt->heading;
        });
        ctx.registry.patch<RenderPosition>(entity, [&](RenderPosition& rp) {
            rp.startX        = rp.x;
            rp.startY        = rp.y;
            rp.startZ        = rp.z;
            rp.startHeading  = rp.heading;
            rp.targetHeading = pkt->heading * 45.f;
            rp.moveStartTime = (float)GetTime();
        });
    }

    void handleChat(WorldContext& ctx, void* pData)
    {
        auto* pkt = reinterpret_cast<const SPacketChat*>(pData);
        auto it = ctx.net.netToEnt.find(pkt->netId);
        if (it == ctx.net.netToEnt.end()) return;

        char buf[129] = {};
        memcpy(buf, pkt->text, 128);
        ctx.registry.emplace_or_replace<ChatBubble>(it->second, std::string(buf), (float)GetTime() + 5.f);
    }
}

void System::NetReceive(WorldContext& ctx)
{
    ctx.net.sockets->RunCallbacks();

    if (ctx.net.connState == NetConnState::Connecting || ctx.net.connState == NetConnState::Disconnected) {
        return;
    }

    ISteamNetworkingMessage* msgs[64];
    int count = ctx.net.sockets->ReceiveMessagesOnConnection(ctx.net.conn, msgs, 64);
    for (int i = 0; i < count; i++) {
        ISteamNetworkingMessage* msg = msgs[i];
        if (msg->m_cbSize < (int)sizeof(PacketHeader)) {
            msg->Release();
            continue;
        }
        auto* hdr = reinterpret_cast<const PacketHeader*>(msg->m_pData);
        switch (hdr->type) {
        case PacketType::SSpawn:
            handleSpawn(ctx, msg->m_pData);
            break;
        case PacketType::SSelf:
            handleSelf(ctx, msg->m_pData);
            break;
        case PacketType::SDespawn:
            handleDespawn(ctx, msg->m_pData);
            break;
        case PacketType::SPosition:
            handlePosition(ctx, msg->m_pData);
            break;
        case PacketType::SChat:
            handleChat(ctx, msg->m_pData);
            break;
        }
        msg->Release();
    }
}

void System::NetSend(WorldContext& ctx)
{
    if (ctx.net.connState == NetConnState::Connecting || ctx.net.connState == NetConnState::Disconnected) {
        return;
    }

    for (auto& packet : ctx.net.outbox.packets) {
        ctx.net.sockets->SendMessageToConnection(
            ctx.net.conn,
            packet.data(),
            (uint32_t)packet.size(),
            k_nSteamNetworkingSend_Reliable,
            nullptr
        );
    }
    ctx.net.outbox.packets.clear();
}
