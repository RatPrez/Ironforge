#pragma once
#include <entt/entt.hpp>
#include "DebugTerminal.hpp"

#include "AssetCache.hpp"
#include "NetContext.hpp"
#include "CamSettings.hpp"

struct Camera3D;

class Game
{
public:
    Game();
    ~Game();
    void tick(const float& dt, Camera3D& camera);
    void tickLate(const float& dt, Camera3D& camera);
    void draw2D(const float& dt, Camera3D& camera);
    void draw3D(const float& dt, Camera3D& camera);

    static Game* Instance() { return s_instance; }

private:
    static void onNetStatusChanged(SteamNetConnectionStatusChangedCallback_t *info);

private:
    entt::registry m_world;
    DebugTerminal m_terminal;

    AssetCache m_assetCache;
    NetContext m_net;
    CamSettings m_camSettings;

    static Game* s_instance;

};
