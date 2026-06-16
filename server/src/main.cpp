#include "base/Server.hpp"

int main()
{
    Server server;
    server.run();
    return 0;
}




// #include <steam/steamnetworkingsockets.h>
// #include <steam/isteamnetworkingutils.h>
// #include <shared/messages.h>

// #include <cstdio>
// #include <cstring>
// #include <csignal>
// #include <thread>
// #include <chrono>
// #include <algorithm>
// #include <vector>

// static bool g_running = true;
// static ISteamNetworkingSockets *g_sockets = nullptr;
// static HSteamListenSocket g_listen_sock = k_HSteamListenSocket_Invalid;
// static HSteamNetPollGroup g_poll_group = k_HSteamNetPollGroup_Invalid;
// static std::vector<HSteamNetConnection> g_clients;

// static void OnSteamNetConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t *info) {
//     switch (info->m_info.m_eState) {
//     case k_ESteamNetworkingConnectionState_Connecting:
//         if (g_sockets->AcceptConnection(info->m_hConn) != k_EResultOK) {
//             g_sockets->CloseConnection(info->m_hConn, 0, nullptr, false);
//             break;
//         }
//         g_sockets->SetConnectionPollGroup(info->m_hConn, g_poll_group);
//         g_clients.push_back(info->m_hConn);
//         printf("[server] client connected: %u\n", info->m_hConn);
//         break;

//     case k_ESteamNetworkingConnectionState_ClosedByPeer:
//     case k_ESteamNetworkingConnectionState_ProblemDetectedLocally:
//         printf("[server] client disconnected: %u\n", info->m_hConn);
//         g_sockets->CloseConnection(info->m_hConn, 0, nullptr, false);
//         g_clients.erase(std::remove(g_clients.begin(), g_clients.end(), info->m_hConn), g_clients.end());
//         break;

//     default:
//         break;
//     }
// }

// static void PollMessages() {
//     ISteamNetworkingMessage *msgs[16];
//     int count = g_sockets->ReceiveMessagesOnPollGroup(g_poll_group, msgs, 16);
//     for (int i = 0; i < count; ++i) {
//         ISteamNetworkingMessage *msg = msgs[i];
//         if (msg->m_cbSize >= (int)sizeof(MessageHeader)) {
//             auto *hdr = reinterpret_cast<const MessageHeader *>(msg->m_pData);
//             if (hdr->type == MessageType::Chat) {
//                 auto *cm = reinterpret_cast<const ChatMessage *>(msg->m_pData);
//                 char buf[257] = {};
//                 memcpy(buf, cm->text, 256);
//                 printf("[server] chat from %u: %s\n", msg->m_conn, buf);


//             }
//         }
//         msg->Release();
//     }
// }

// int main() {
//     SteamDatagramErrMsg err;
//     if (!GameNetworkingSockets_Init(nullptr, err)) {
//         fprintf(stderr, "GameNetworkingSockets_Init failed: %s\n", err);
//         return 1;
//     }

//     g_sockets = SteamNetworkingSockets();

//     SteamNetworkingIPAddr addr;
//     addr.Clear();
//     addr.m_port = 27020;

//     SteamNetworkingConfigValue_t opt;
//     opt.SetPtr(k_ESteamNetworkingConfig_Callback_ConnectionStatusChanged, (void *)OnSteamNetConnectionStatusChanged);

//     g_listen_sock = g_sockets->CreateListenSocketIP(addr, 1, &opt);
//     if (g_listen_sock == k_HSteamListenSocket_Invalid) {
//         fprintf(stderr, "Failed to create listen socket\n");
//         return 1;
//     }

//     g_poll_group = g_sockets->CreatePollGroup();

//     printf("[server] listening on port 27020\n");

//     signal(SIGINT,  [](int) { g_running = false; });
//     signal(SIGTERM, [](int) { g_running = false; });

//     while (g_running) {
//         g_sockets->RunCallbacks();
//         PollMessages();
//         std::this_thread::sleep_for(std::chrono::milliseconds(10));
//     }

//     g_sockets->CloseListenSocket(g_listen_sock);
//     g_sockets->DestroyPollGroup(g_poll_group);
//     GameNetworkingSockets_Kill();
//     return 0;
// }
