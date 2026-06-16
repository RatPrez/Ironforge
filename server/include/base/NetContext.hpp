#pragma once
#include <unordered_map>
#include <vector>

#include <entt/entity/entity.hpp>
#include <steam/steamnetworkingsockets.h>

struct NetOutbox
{
    std::unordered_map<HSteamNetConnection, std::vector<std::vector<uint8_t>>> packets;

    template<typename T>
    void send(HSteamNetConnection conn, const T& pkt)
    {
        auto* begin = reinterpret_cast<const uint8_t*>(&pkt);
        packets[conn].emplace_back(begin, begin + sizeof(T));
    }

    template<typename T>
    void broadcast(const std::vector<HSteamNetConnection>& clients, const T& pkt,
                   HSteamNetConnection exclude = k_HSteamNetConnection_Invalid)
    {
        for (HSteamNetConnection conn : clients) {
            if (conn != exclude)
                send(conn, pkt);
        }
    }
};

struct NetContext
{
    ISteamNetworkingSockets* sockets = nullptr;
    HSteamListenSocket listenSocket  = k_HSteamListenSocket_Invalid;
    HSteamNetPollGroup pollGroup     = k_HSteamNetPollGroup_Invalid;

    std::vector<HSteamNetConnection> clients;
    std::unordered_map<HSteamNetConnection, entt::entity> connToEntity;

    NetOutbox outbox;
};
