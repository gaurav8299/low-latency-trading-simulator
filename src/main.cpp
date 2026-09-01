#include "common/types.hpp"
#include "common/constants.hpp"
#include "common/logger.hpp"

#include <iostream>
#include <string>
#include <string_view>
#include <vector>
#include <algorithm>

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

hft::TradingMode parse_mode(std::string_view mode_str) {
    std::string mode_upper;
    mode_upper.reserve(mode_str.size());
    for (char c : mode_str) {
        mode_upper += static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
    }

    if (mode_upper == "PAPER") {
        return hft::TradingMode::Paper;
    }

    if (mode_upper != "SIMULATION") {
        std::cerr << "[WARNING] Unrecognized mode '" << mode_str
                  << "'. Defaulting to SIMULATION.\n";
    }
    return hft::TradingMode::Simulation;
}

int main(int argc, char* argv[]) {
    std::vector<std::string_view> args;
    for (int i = 1; i < argc; ++i) {
        args.emplace_back(argv[i]);
    }

    if (std::find(args.begin(), args.end(), "--help") != args.end()) {
        print_help();
        return 0;
    }

    hft::TradingMode mode = hft::TradingMode::Simulation;
    for (size_t i = 0; i < args.size(); ++i) {
        if (args[i] == "--mode" && i + 1 < args.size()) {
            mode = parse_mode(args[i + 1]);
        }
    }

    bool verbose = std::find(args.begin(), args.end(), "--verbose") != args.end();

    std::cout << hft::constants::DISCLAIMER;

    auto& logger = hft::Logger::instance();
    logger.set_level(verbose ? hft::LogLevel::Debug : hft::LogLevel::Info);

    logger.info("Main", std::string("Version: ") + std::string(hft::constants::VERSION));
    logger.info("Main", std::string("Trading Mode: ") + std::string(hft::to_string(mode)));

    if (mode == hft::TradingMode::Simulation) {
        logger.info("Main", "SIMULATION MODE — No real orders will be placed.");
    } else if (mode == hft::TradingMode::Paper) {
        logger.info("Main", "PAPER TRADING MODE — No real orders will be placed.");
    }

    logger.info("Main", "System initialized successfully.");
    logger.info("Main", "Shutting down cleanly.");

    std::cout << "\n[OK] Simulator exited cleanly.\n";
    return 0;
}
