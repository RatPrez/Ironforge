#pragma once
#include <string>
#include <vector>
#include <functional>
#include <unordered_map>

enum class LogLevel { Info, Warning, Error };

struct LogEntry
{
    LogLevel    level;
    std::string message;
};

using CommandHandler = std::function<void(const std::vector<std::string>& args)>;

class DebugTerminal
{
public:
    DebugTerminal();
    ~DebugTerminal() = default;

    void draw();
    void log(const std::string& message, LogLevel level = LogLevel::Info);
    void registerCommand(const std::string& name, CommandHandler handler);

    static DebugTerminal* Instance() { return s_instance; }

private:
    void executeCommand(const std::string& input);

    static DebugTerminal* s_instance;

    bool                  m_visible = false;
    std::vector<LogEntry> m_entries;
    char                  m_inputBuf[256] = {};
    std::unordered_map<std::string, CommandHandler> m_commands;
};
