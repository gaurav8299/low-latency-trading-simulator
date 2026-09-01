#pragma once
// ==============================================================================
// logger.hpp — Structured Logger
// ==============================================================================
//
// WHAT: A simple logging class that writes timestamped, leveled messages.
// WHY:  We need to record everything that happens in the simulator —
//        every order, every fill, every risk decision. Logs are how we
//        debug problems and understand system behavior.
// HOW:  Logger::instance().info("Order filled", "order_id=123");
//
// IMPORTANT:
//   - This logger NEVER logs API secrets, passwords, or credentials.
//   - By design, it only accepts string messages — no credential types.
//   - In production, you'd use a proper logging library (spdlog, etc.)
//     but this simple version teaches the concepts.
//
// DESIGN PATTERN: Singleton
//   We use a singleton so there's exactly one logger in the whole program.
//   Every component writes to the same log. This makes debugging easier.
//
// ==============================================================================

#include <string>
#include <string_view>
#include <iostream>
#include <fstream>
#include <mutex>
#include <chrono>
#include <format>

namespace hft {

// ==============================================================================
// Log Level — How important is this message?
// ==============================================================================
// DEBUG:    Developer details (very verbose)
// INFO:     Normal operation events
// WARNING:  Something unusual but not broken
// ERROR:    Something went wrong
// CRITICAL: System must stop (kill switch, etc.)
// ==============================================================================
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

// ==============================================================================
// Logger Class
// ==============================================================================
class Logger {
public:
    // ------------------------------------------------------------------
    // Singleton access
    // ------------------------------------------------------------------
    // "static" means there's one instance shared by the whole program.
    // This is called the "Meyers Singleton" — it's thread-safe in C++11+.
    // ------------------------------------------------------------------
    static Logger& instance() {
        static Logger logger;
        return logger;
    }

    // ------------------------------------------------------------------
    // Configuration
    // ------------------------------------------------------------------
    void set_level(LogLevel level) { min_level_ = level; }
    void set_console_output(bool enabled) { console_output_ = enabled; }

    // Open a log file. Returns true on success.
    bool open_file(const std::string& filepath) {
        std::lock_guard<std::mutex> lock(mutex_);
        file_.open(filepath, std::ios::out | std::ios::app);
        return file_.is_open();
    }

    // ------------------------------------------------------------------
    // Logging methods — one per level
    // ------------------------------------------------------------------
    // string_view is a lightweight non-owning reference to a string.
    // It avoids copying the string just to log it.
    // ------------------------------------------------------------------
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

    // ------------------------------------------------------------------
    // Core log method
    // ------------------------------------------------------------------
    void log(LogLevel level, std::string_view component, std::string_view message);

    // Prevent copying — there should only be one Logger
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

private:
    // Private constructor — only instance() can create a Logger
    Logger() = default;
    ~Logger() {
        if (file_.is_open()) {
            file_.close();
        }
    }

    // ------------------------------------------------------------------
    // Member variables
    // ------------------------------------------------------------------
    LogLevel      min_level_       = LogLevel::Info;  // Default: show Info and above
    bool          console_output_  = true;            // Print to terminal
    std::ofstream file_;                              // Optional log file
    std::mutex    mutex_;                             // Thread safety
};

} // namespace hft
