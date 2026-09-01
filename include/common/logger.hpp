#pragma once

#include <string>
#include <string_view>
#include <iostream>
#include <fstream>
#include <mutex>
#include <chrono>

namespace hft {

enum class LogLevel {
    Debug,
    Info,
    Warning,
    Error,
    Critical
};

[[nodiscard]] constexpr std::string_view to_string(LogLevel level) {
    switch (level) {
        case LogLevel::Debug:    return "DEBUG";
        case LogLevel::Info:     return "INFO";
        case LogLevel::Warning:  return "WARNING";
        case LogLevel::Error:    return "ERROR";
        case LogLevel::Critical: return "CRITICAL";
    }
    return "UNKNOWN";
}

class Logger {
public:
    static Logger& instance() {
        static Logger logger;
        return logger;
    }

    void set_level(LogLevel level) { min_level_ = level; }
    void set_console_output(bool enabled) { console_output_ = enabled; }

    bool open_file(const std::string& filepath) {
        std::lock_guard<std::mutex> lock(mutex_);
        file_.open(filepath, std::ios::out | std::ios::app);
        return file_.is_open();
    }

    void debug(std::string_view component, std::string_view message) {
        log(LogLevel::Debug, component, message);
    }

    void info(std::string_view component, std::string_view message) {
        log(LogLevel::Info, component, message);
    }

    void warning(std::string_view component, std::string_view message) {
        log(LogLevel::Warning, component, message);
    }

    void error(std::string_view component, std::string_view message) {
        log(LogLevel::Error, component, message);
    }

    void critical(std::string_view component, std::string_view message) {
        log(LogLevel::Critical, component, message);
    }

    void log(LogLevel level, std::string_view component, std::string_view message);

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

private:
    Logger() = default;
    ~Logger() {
        if (file_.is_open()) {
            file_.close();
        }
    }

    LogLevel      min_level_       = LogLevel::Info;
    bool          console_output_  = true;
    std::ofstream file_;
    std::mutex    mutex_;
};

} // namespace hft
