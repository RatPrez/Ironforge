#include "base/DebugTerminal.hpp"
#include <raylib.h>
#include <rlImGui.h>
#include <imgui.h>

DebugTerminal* DebugTerminal::s_instance = nullptr;

DebugTerminal::DebugTerminal()
{
    s_instance = this;
}

void DebugTerminal::log(const std::string& message, LogLevel level)
{
    m_entries.push_back({ level, message });
}

void DebugTerminal::draw()
{
    if (IsKeyPressed(KEY_GRAVE)) m_visible = !m_visible;
    if (!m_visible) return;

    ImGui::SetNextWindowSize({ 600, 300 }, ImGuiCond_FirstUseEver);
    ImGui::Begin("Debug Terminal", &m_visible);

    ImGui::BeginChild("entries", { 0, -ImGui::GetFrameHeightWithSpacing() }, false, ImGuiWindowFlags_HorizontalScrollbar);
    for (const auto& entry : m_entries) {
        switch (entry.level) {
        case LogLevel::Info:
            ImGui::TextUnformatted(entry.message.c_str());
            break;
        case LogLevel::Warning:
            ImGui::TextColored({ 1.f, 1.f, 0.f, 1.f }, "%s", entry.message.c_str());
            break;
        case LogLevel::Error:
            ImGui::TextColored({ 1.f, 0.f, 0.f, 1.f }, "%s", entry.message.c_str());
            break;
        }
    }
    if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
        ImGui::SetScrollHereY(1.0f);
    }
    ImGui::EndChild();

    ImGui::End();
}
