#pragma once
#include <unordered_map>
#include <vector>

#include <entt/entity/entity.hpp>
#include <steam/steamnetworkingsockets.h>

enum class NetConnState { Connecting, Connected, Disconnected };

struct NetOutbox
{
    std::vector<std::vector<uint8_t>> packets;

    template<typename T>
    void push(const T& pkt) {
        auto* begin = reinterpret_cast<const uint8_t*>(&pkt);
        packets.emplace_back(begin, begin + sizeof(T));
    }
};

struct NetContext
{
    ISteamNetworkingSockets* sockets  = nullptr;
    HSteamNetConnection conn = k_HSteamNetConnection_Invalid;
    NetConnState connState = NetConnState::Disconnected;
    std::unordered_map<uint32_t, entt::entity> netToEnt;
    NetOutbox outbox;
};
