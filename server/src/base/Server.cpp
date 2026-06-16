#include "base/Server.hpp"
#include <csignal>
#include <chrono>
#include <thread>

#include "Systems.hpp"
#include "base/WorldContext.hpp"
#include "shared/Packets.hpp"
#include "shared/Components.hpp"
#include "Components.hpp"

Server* Server::s_instance = nullptr;

Server::Server()
    : m_running(true)
{
    s_instance = this;

    SteamDatagramErrMsg err;
    if (!GameNetworkingSockets_Init(nullptr, err)) {
        fprintf(stderr, "GameNetworkingSockets_Init failed: %s\n", err);
        return;
    }

    m_net.sockets = SteamNetworkingSockets();

    SteamNetworkingIPAddr addr;
    addr.Clear();
    addr.m_port = 27020;

    SteamNetworkingConfigValue_t opt;
    opt.SetPtr(k_ESteamNetworkingConfig_Callback_ConnectionStatusChanged, (void *)onNetStatusChanged);

    m_net.listenSocket = m_net.sockets->CreateListenSocketIP(addr, 1, &opt);
    if (m_net.listenSocket == k_HSteamListenSocket_Invalid) {
        fprintf(stderr, "Failed to create listen socket\n");
        return;
    }

    m_net.pollGroup = m_net.sockets->CreatePollGroup();

    printf("[server] listening on port 27020\n");

    signal(SIGINT,  [](int) { if (s_instance) s_instance->m_running = false; });
    signal(SIGTERM, [](int) { if (s_instance) s_instance->m_running = false; });
}

Server::~Server()
{
    m_net.sockets->CloseListenSocket(m_net.listenSocket);
    m_net.sockets->DestroyPollGroup(m_net.pollGroup);
    GameNetworkingSockets_Kill();
}

void Server::onNetStatusChanged(SteamNetConnectionStatusChangedCallback_t *info)
{
    switch (info->m_info.m_eState) {
    case k_ESteamNetworkingConnectionState_Connecting:
        if (s_instance->m_net.sockets->AcceptConnection(info->m_hConn) != k_EResultOK) {
            s_instance->m_net.sockets->CloseConnection(info->m_hConn, 0, nullptr, false);
            break;
        }
        s_instance->m_net.sockets->SetConnectionPollGroup(info->m_hConn, s_instance->m_net.pollGroup);
        s_instance->m_net.clients.push_back(info->m_hConn);
        {
            entt::entity entity = s_instance->m_world.create();
            s_instance->m_net.connToEntity[info->m_hConn] = entity;
            s_instance->m_world.emplace<Position>(entity, uint16_t{20}, uint16_t{20}, uint8_t{0});
            s_instance->m_world.emplace<Player>(entity, info->m_hConn);
            s_instance->m_world.emplace<KnownEntities>(entity);

            SPacketSelf selfPkt;
            selfPkt.netId = static_cast<uint32_t>(entity);
            s_instance->m_net.outbox.send(info->m_hConn, selfPkt);

            printf("[server] client connected: %u, entity: %u\n", info->m_hConn, (uint32_t)entity);
        }
        break;

    case k_ESteamNetworkingConnectionState_ClosedByPeer:
    case k_ESteamNetworkingConnectionState_ProblemDetectedLocally:
        printf("[server] client disconnected: %u\n", info->m_hConn);
        s_instance->m_net.sockets->CloseConnection(info->m_hConn, 0, nullptr, false);
        {
            auto it = s_instance->m_net.connToEntity.find(info->m_hConn);
            if (it != s_instance->m_net.connToEntity.end()) {
                s_instance->m_world.destroy(it->second);
                s_instance->m_net.connToEntity.erase(it);
            }
        }
        s_instance->m_net.clients.erase(
            std::remove(
                s_instance->m_net.clients.begin(),
                s_instance->m_net.clients.end(),
                info->m_hConn
            ),
            s_instance->m_net.clients.end()
        );
        break;

    default:
        break;
    }
}

void Server::run()
{
    constexpr auto kTickDuration = std::chrono::milliseconds(600);

    while (m_running) {
        auto tick_start = std::chrono::steady_clock::now();

        WorldContext ctx{ m_world, m_net };
        System::NetReceive(ctx);
        System::Pathfinder(ctx);
        System::Movement(ctx);
        System::Sync(ctx);
        System::NetSend(ctx);

        auto elapsed = std::chrono::steady_clock::now() - tick_start;
        auto sleep_time = kTickDuration - elapsed;
        if (sleep_time > std::chrono::milliseconds(0)) {
            std::this_thread::sleep_for(sleep_time);
        }
    }
}
