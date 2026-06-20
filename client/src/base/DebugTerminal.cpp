#include "base/DebugTerminal.hpp"
#include <raylib.h>
#include <rlImGui.h>
#include <imgui.h>
#include <sstream>

DebugTerminal* DebugTerminal::s_instance = nullptr;

DebugTerminal::DebugTerminal()
{
    s_instance = this;
}

void DebugTerminal::log(const std::string& message, LogLevel level)
{
    m_entries.push_back({ level, message });
}

void DebugTerminal::registerCommand(const std::string& name, CommandHandler handler)
{
    m_commands[name] = std::move(handler);
}

void DebugTerminal::executeCommand(const std::string& input)
{
    std::string line = input;
    if (!line.empty() && line[0] == '/') line = line.substr(1);

    std::istringstream ss(line);
    std::string name;
    ss >> name;

    std::vector<std::string> args;
    std::string arg;
    while (ss >> arg) args.push_back(arg);

    auto it = m_commands.find(name);
    if (it != m_commands.end()) {
        it->second(args);
    } else {
        log("Unknown command: " + name, LogLevel::Warning);
    }
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

    ImGui::SetNextItemWidth(-1.f);
    if (ImGui::InputText("##input", m_inputBuf, sizeof(m_inputBuf), ImGuiInputTextFlags_EnterReturnsTrue)) {
        if (m_inputBuf[0] != '\0') {
            log(m_inputBuf);
            executeCommand(m_inputBuf);
        }
        m_inputBuf[0] = '\0';
        ImGui::SetKeyboardFocusHere(-1);
    }

    ImGui::End();
}
