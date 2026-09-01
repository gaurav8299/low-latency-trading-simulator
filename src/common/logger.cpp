// ==============================================================================
// logger.cpp — Logger Implementation
// ==============================================================================
//
// WHAT: The actual code that writes log messages.
// WHY:  We separate the implementation (.cpp) from the interface (.hpp) so that:
//        1. Changing the implementation doesn't recompile everything
//        2. The header stays clean and readable
//        3. Compile times are faster
// HOW:  This file is compiled once and linked into the library.
//
// ==============================================================================

#include "common/logger.hpp"

#include <iomanip>
#include <sstream>

namespace hft {

void Logger::log(LogLevel level, std::string_view component, std::string_view message) {
    // ------------------------------------------------------------------
    // Filter: skip messages below our minimum level
    // ------------------------------------------------------------------
    // If min_level_ is Info, then Debug messages are silently ignored.
    // This uses the underlying integer values of the enum.
    // ------------------------------------------------------------------
    if (level < min_level_) {
        return;
    }

    // ------------------------------------------------------------------
    // Timestamp: when did this event happen?
    // ------------------------------------------------------------------
    // system_clock gives wall-clock time (what time is it?).
    // steady_clock gives monotonic time (for measuring durations).
    // We use system_clock here because logs need human-readable times.
    // ------------------------------------------------------------------
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    auto us = std::chrono::duration_cast<std::chrono::microseconds>(
        now.time_since_epoch()) % 1000000;

    // ------------------------------------------------------------------
    // Format the log line
    // ------------------------------------------------------------------
    // Format: [TIMESTAMP] [LEVEL] [COMPONENT] Message
    //
    // Example:
    // [2024-01-15 10:30:45.123456] [INFO] [OrderBook] New limit buy: 100@50.25
    // ------------------------------------------------------------------
    std::ostringstream oss;

    // Thread-safe time formatting
    std::tm tm_buf{};
#if defined(_MSC_VER) || defined(_WIN32)
    localtime_s(&tm_buf, &time_t_now);
#else
    localtime_r(&time_t_now, &tm_buf);
#endif

    oss << '['
        << std::put_time(&tm_buf, "%Y-%m-%d %H:%M:%S")
        << '.' << std::setfill('0') << std::setw(6) << us.count()
        << "] ["
        << to_string(level)
        << "] ["
        << component
        << "] "
        << message;

    std::string formatted = oss.str();

    // ------------------------------------------------------------------
    // Write the log line (thread-safe)
    // ------------------------------------------------------------------
    // lock_guard is RAII for mutexes: it locks when created, unlocks when
    // destroyed (when this block ends). This prevents forgetting to unlock.
    // ------------------------------------------------------------------
    std::lock_guard<std::mutex> lock(mutex_);

    if (console_output_) {
        // Use cerr for warnings/errors so they appear even if stdout is redirected
        if (level >= LogLevel::Warning) {
            std::cerr << formatted << '\n';
        } else {
            std::cout << formatted << '\n';
        }
    }

    if (file_.is_open()) {
        file_ << formatted << '\n';
        file_.flush();  // Ensure log is written even if we crash
    }
}

} // namespace hft
