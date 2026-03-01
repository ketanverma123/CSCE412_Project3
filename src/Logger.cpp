#include "Logger.h"
#include <iostream>

Logger::~Logger() 
{
    if (logfile_.is_open()) logfile_.close();
}

bool Logger::open(const std::string& filepath) 
{
    logfile_.open(filepath, std::ios::out | std::ios::trunc);
    return logfile_.is_open();
}

bool Logger::isOpen() const { return logfile_.is_open(); }

std::string Logger::colorReset() const { return "\033[0m"; }

std::string Logger::colorPrefix(LogKind logtype) const {
    switch (logtype) 
    {
        case LogKind::REQUEST_ARRIVAL:return "\033[36m"; 
        case LogKind::REQUEST_ASSIGN:return "\033[34m"; 
        case LogKind::REQUEST_COMPLETE:return "\033[32m"; 
        case LogKind::SERVER_SCALE_UP:return "\033[33m"; 
        case LogKind::SERVER_SCALE_DOWN:return "\033[35m"; 
        case LogKind::FIREWALL_BLOCK:return "\033[31m"; 
        case LogKind::STATUS:return "\033[90m"; 
        default:return "\033[37m"; 
    }
}

std::string Logger::kindPrefix(LogKind logtype) const 
{
    switch (logtype) 
    {
        case LogKind::LOG_INFO:return "[LOG_INFO] ";
        case LogKind::REQUEST_ARRIVAL:return "[REQUEST_ARRIVAL] ";
        case LogKind::REQUEST_ASSIGN:return "[REQUEST_ASSIGN] ";
        case LogKind::REQUEST_COMPLETE:return "[REQUEST_COMPLETE] ";
        case LogKind::SERVER_SCALE_UP:return "[SERVER_SCALE_UP] ";
        case LogKind::SERVER_SCALE_DOWN:return "[SERVER_SCALE_DOWN] ";
        case LogKind::FIREWALL_BLOCK:return "[FIREWALL_BLOCK] ";
        case LogKind::STATUS:return "[STATUS] ";
        default:return "[LOG] ";
    }
}

void Logger::log(LogKind logtype, const std::string& line, bool consoleprint, bool coloruse) 
{
    const std::string label = kindPrefix(logtype);
    if (logfile_.is_open()) logfile_ << label << line << "\n";

    if (consoleprint) 
    {
        if (coloruse) std::cout << colorPrefix(logtype) << label << line << colorReset() << "\n";
        else std::cout << label << line << "\n";
    }
}

void Logger::raw(const std::string& msg, bool toConsole) 
{
    if (logfile_.is_open()) logfile_ << msg << "\n";
    if (toConsole) std::cout << msg << "\n";
}