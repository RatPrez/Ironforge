#include "system/Chat.hpp"
#include <raylib.h>
#include <imgui.h>
#include <cstring>

#include "shared/Packets.hpp"
#include "base/NetContext.hpp"

namespace
{
    bool g_chatOpen = false;
    char g_chatBuffer[128] = {};
}

void System::Chat(WorldContext &ctx, const float &dt)
{
    if (!g_chatOpen && IsKeyPressed(KEY_ENTER)) {
        g_chatOpen = true;
        return;
    }

    if (!g_chatOpen) return;

    ImGui::SetNextWindowPos({ 10.f, (float)GetScreenHeight() - 40.f });
    ImGui::SetNextWindowSize({ 300.f, 0.f });
    ImGui::Begin("##chatinput", nullptr,
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

    ImGui::SetKeyboardFocusHere();
    bool sent = ImGui::InputText("##chat", g_chatBuffer, sizeof(g_chatBuffer), ImGuiInputTextFlags_EnterReturnsTrue);

    ImGui::End();

    if (IsKeyPressed(KEY_ESCAPE)) {
        g_chatOpen = false;
        g_chatBuffer[0] = '\0';
        return;
    }

    if (sent) {
        if (g_chatBuffer[0] != '\0') {
            CPacketChat pkt;
            memset(pkt.text, 0, sizeof(pkt.text));
            strncpy(pkt.text, g_chatBuffer, sizeof(pkt.text) - 1);
            ctx.net.outbox.push(pkt);
        }
        g_chatOpen = false;
        g_chatBuffer[0] = '\0';
    }
}
