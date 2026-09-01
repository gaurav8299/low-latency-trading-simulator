// ==============================================================================
// main.cpp — Entry Point for the HFT-Style Trading Simulator
// ==============================================================================
//
// WHAT: This is where the program starts executing.
// WHY:  Every C++ program needs exactly one main() function.
// HOW:  The OS calls main() when you run ./hft_simulator
//
// This file handles:
//   1. Parsing command-line arguments (--mode, --help)
//   2. Displaying the mandatory disclaimer
//   3. Setting up the logger
//   4. Starting the simulator (placeholder for now)
//
// ==============================================================================

#include "common/types.hpp"
#include "common/constants.hpp"
#include "common/logger.hpp"

#include <iostream>
#include <string>
#include <string_view>
#include <vector>
#include <algorithm>

// ==============================================================================
// Helper: Print usage/help text
// ==============================================================================
void print_help() {
    std::cout << "\n"
              << hft::constants::PROJECT_NAME << " v" << hft::constants::VERSION << "\n"
              << "\n"
              << "Usage:\n"
              << "  hft_simulator [options]\n"
              << "\n"
              << "Options:\n"
              << "  --mode <mode>   Trading mode: simulation (default), paper\n"
              << "  --config <file> Path to configuration file\n"
              << "  --verbose       Enable debug-level logging\n"
              << "  --help          Show this help message\n"
              << "\n"
              << "Examples:\n"
              << "  hft_simulator --mode simulation\n"
              << "  hft_simulator --mode paper --config config/simulation.yaml\n"
              << "  hft_simulator --verbose\n"
              << "\n"
              << "IMPORTANT:\n"
              << "  Real-money trading is DISABLED. This is a simulator only.\n"
              << "  See README.md for documentation.\n"
              << "\n";
}

// ==============================================================================
// Helper: Parse trading mode from string
// ==============================================================================
// Returns the TradingMode enum, or Simulation if the string is unrecognized.
// This ensures we NEVER accidentally enter a live trading mode.
// ==============================================================================
hft::TradingMode parse_mode(std::string_view mode_str) {
    // Convert to uppercase for case-insensitive comparison
    std::string mode_upper;
    mode_upper.reserve(mode_str.size());
    for (char c : mode_str) {
        mode_upper += static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
    }

    if (mode_upper == "PAPER") {
        return hft::TradingMode::Paper;
    }

    // Default to Simulation for ANY unrecognized input.
    // This is a safety measure — we never accidentally go live.
    if (mode_upper != "SIMULATION") {
        std::cerr << "[WARNING] Unrecognized mode '" << mode_str
                  << "'. Defaulting to SIMULATION.\n";
    }
    return hft::TradingMode::Simulation;
}

// ==============================================================================
// Main function
// ==============================================================================
int main(int argc, char* argv[]) {
    // ------------------------------------------------------------------
    // Step 1: Parse command-line arguments
    // ------------------------------------------------------------------
    // We convert argv (raw C-style array) into a vector of string_view
    // for safer, easier handling.
    //
    // argv[0] is the program name, argv[1..] are the arguments.
    // ------------------------------------------------------------------
    std::vector<std::string_view> args;
    for (int i = 1; i < argc; ++i) {
        args.emplace_back(argv[i]);
    }

    // Check for --help first
    if (std::find(args.begin(), args.end(), "--help") != args.end()) {
        print_help();
        return 0;
    }

    // Parse --mode
    hft::TradingMode mode = hft::TradingMode::Simulation;  // SAFE DEFAULT
    for (size_t i = 0; i < args.size(); ++i) {
        if (args[i] == "--mode" && i + 1 < args.size()) {
            mode = parse_mode(args[i + 1]);
        }
    }

    // Parse --verbose
    bool verbose = std::find(args.begin(), args.end(), "--verbose") != args.end();

    // ------------------------------------------------------------------
    // Step 2: Display the mandatory disclaimer
    // ------------------------------------------------------------------
    // This MUST appear every time the program starts.
    // It reminds the user that this is NOT real trading.
    // ------------------------------------------------------------------
    std::cout << hft::constants::DISCLAIMER;

    // ------------------------------------------------------------------
    // Step 3: Set up the logger
    // ------------------------------------------------------------------
    auto& logger = hft::Logger::instance();
    logger.set_level(verbose ? hft::LogLevel::Debug : hft::LogLevel::Info);

    // ------------------------------------------------------------------
    // Step 4: Log startup information
    // ------------------------------------------------------------------
    logger.info("Main", std::string("Version: ") + std::string(hft::constants::VERSION));
    logger.info("Main", std::string("Trading Mode: ") + std::string(hft::to_string(mode)));

    // Safety confirmation
    if (mode == hft::TradingMode::Simulation) {
        logger.info("Main", "SIMULATION MODE — No real orders will be placed.");
    } else if (mode == hft::TradingMode::Paper) {
        logger.info("Main", "PAPER TRADING MODE — No real orders will be placed.");
    }

    // ------------------------------------------------------------------
    // Step 5: Placeholder for the main simulation loop
    // ------------------------------------------------------------------
    // In Phase 1, we just confirm the system starts correctly.
    // Future phases will add:
    //   - Configuration loading
    //   - Market data replay
    //   - Strategy execution
    //   - Order processing
    //   - Results reporting
    // ------------------------------------------------------------------
    logger.info("Main", "System initialized successfully.");
    logger.info("Main", "Phase 1 complete — skeleton is working!");
    logger.info("Main", "Shutting down cleanly.");

    std::cout << "\n[OK] Simulator exited cleanly.\n";
    return 0;
}
