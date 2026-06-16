#pragma once
#include <string>
#include <vector>

enum class LogLevel { Info, Warning, Error };

struct LogEntry
{
    LogLevel    level;
    std::string message;
};

class DebugTerminal
{
public:
    DebugTerminal();
    ~DebugTerminal() = default;

    void draw();
    void log(const std::string& message, LogLevel level = LogLevel::Info);

    static DebugTerminal* Instance() { return s_instance; }

private:
    static DebugTerminal* s_instance;

    bool                  m_visible = false;
    std::vector<LogEntry> m_entries;
};
