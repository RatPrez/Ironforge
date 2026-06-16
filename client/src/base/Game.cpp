#include "base/Game.hpp"
#include "base/WorldContext.hpp"
#include <raylib.h>
#include <steam/isteamnetworkingutils.h>

#include "Systems.hpp"
#include "Components.hpp"
#include "shared/Components.hpp"

Game* Game::s_instance = nullptr;

void Game::onNetStatusChanged(SteamNetConnectionStatusChangedCallback_t *info)
{
    if (info->m_hConn != s_instance->m_net.conn) return;

    switch (info->m_info.m_eState) {
    case k_ESteamNetworkingConnectionState_Connected:
        DebugTerminal::Instance()->log("Connected to Server");
        s_instance->m_net.connState = NetConnState::Connected;
        break;
    case k_ESteamNetworkingConnectionState_ClosedByPeer:
    case k_ESteamNetworkingConnectionState_ProblemDetectedLocally:
        DebugTerminal::Instance()->log("Disconnected from Server");
        s_instance->m_net.connState = NetConnState::Disconnected;
        break;
    default:
        break;
    }
}

Game::Game()
{
    s_instance = this;

    SteamDatagramErrMsg err;
    if (!GameNetworkingSockets_Init(nullptr, err)) {
        throw std::runtime_error(err);
    }

    m_net.sockets = SteamNetworkingSockets();

    SteamNetworkingConfigValue_t opt;
    opt.SetPtr(
        k_ESteamNetworkingConfig_Callback_ConnectionStatusChanged,
        (void *)onNetStatusChanged
    );

    SteamNetworkingIPAddr addr;
    addr.Clear();
    addr.ParseString("127.0.0.1");
    addr.m_port = 27020;

    m_net.conn = m_net.sockets->ConnectByIPAddress(addr, 1, &opt);
    if (m_net.conn == k_HSteamNetConnection_Invalid) {
        m_terminal.log("Failed to connect to Server", LogLevel::Error);
        return;
    }

    m_terminal.log("Attempting to connect to Server");
}

Game::~Game()
{
    if (m_net.conn != k_HSteamNetConnection_Invalid) {
        m_net.sockets->CloseConnection(m_net.conn, 0, nullptr, false);
    }
    GameNetworkingSockets_Kill();
}

void Game::tick(const float& dt, Camera3D& camera)
{
    WorldContext ctx{ m_world, m_net, camera, m_assetCache, m_camSettings };
    System::NetReceive(ctx);
    System::Camera(ctx, dt);
    System::MovementInterp(ctx, dt);
    System::Input(ctx);
}

void Game::tickLate(const float& dt, Camera3D& camera)
{
    WorldContext ctx{ m_world, m_net, camera, m_assetCache, m_camSettings };
    System::NetSend(ctx);
}

void Game::draw2D()
{
    m_terminal.draw();
}

void Game::draw3D(Camera3D& camera)
{
    WorldContext ctx{ m_world, m_net, camera, m_assetCache, m_camSettings };
    System::RenderWorld(ctx);
    System::RenderActors(ctx);
}
