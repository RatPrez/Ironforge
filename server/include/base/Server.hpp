#pragma once
#include <entt/entt.hpp>
#include "NetContext.hpp"

struct Server
{
public:
    Server();
    ~Server();
    void run();

    static Server* Instance() { return s_instance; }

private:
    static void onNetStatusChanged(SteamNetConnectionStatusChangedCallback_t *info);

private:
    bool m_running;
    NetContext m_net;
    entt::registry m_world;

    static Server* s_instance;
};
