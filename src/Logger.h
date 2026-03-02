/**
 * @file Logger.h
 * @brief Logging utility used by the simulation.
 */
#pragma once
#include <fstream>
#include <string>
/**
 * @file Logger.h
 * @brief Logging utility used by the simulation.
 */
enum class LogKind 
{
    LOG_INFO,
    REQUEST_ARRIVAL,
    REQUEST_ASSIGN,
    REQUEST_COMPLETE,
    SERVER_SCALE_UP,
    SERVER_SCALE_DOWN,
    FIREWALL_BLOCK,
    STATUS
};
/**
 * @class Logger
 * @brief Writes log messages to a file and optionally to console.
 */
class Logger 
{
public:
    Logger()=default;
    ~Logger();
    bool open(const std::string& path);
    void log(LogKind kind, const std::string& line, bool toConsole, bool colorOutput);
    void raw(const std::string& line, bool toConsole);
    bool isOpen() const;

private:
    std::ofstream logfile_;
    std::string colorPrefix(LogKind kind) const;
    std::string kindPrefix(LogKind kind) const;
    std::string colorReset() const;
};