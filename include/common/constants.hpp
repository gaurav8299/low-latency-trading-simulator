#pragma once
// ==============================================================================
// constants.hpp — System-Wide Constants
// ==============================================================================
//
// WHAT: Fixed values that don't change at runtime.
// WHY:  Centralizing constants prevents "magic numbers" scattered in code.
//        If you need to change the version, you change it in ONE place.
// HOW:  #include <common/constants.hpp>
//
// NOTE: constexpr means "known at compile time" — the compiler can optimize
//       these as if they were literals, but they have meaningful names.
//
// ==============================================================================

#include <string_view>

namespace hft::constants {

// ==============================================================================
// Version
// ==============================================================================
constexpr std::string_view VERSION = "0.1.0";
constexpr std::string_view PROJECT_NAME = "HFT-Style Trading Simulator";

// ==============================================================================
// DISCLAIMER — This MUST be displayed on every startup
// ==============================================================================
// This is not optional. Trading simulators can give false confidence.
// Always remind the user that this is simulation.
// ==============================================================================
constexpr std::string_view DISCLAIMER = R"(
================================================================================
  SIMULATION ONLY — NO REAL ORDERS ARE BEING PLACED.

  This is an educational HFT-style trading simulator.
  - Backtest performance is NOT evidence of future profitability.
  - This system does NOT guarantee returns.
  - Real-money trading is DISABLED by default.
  - Never risk money you cannot afford to lose.
================================================================================
)";

// ==============================================================================
// Default Risk Limits
// ==============================================================================
// These are safe defaults. They can be overridden via config/simulation.yaml.
// They exist here so the system is safe even without a config file.
// ==============================================================================
constexpr double   DEFAULT_INITIAL_CAPITAL       = 100000.0;  // $100,000
constexpr double   DEFAULT_MAX_ORDER_VALUE        = 10000.0;   // $10,000
constexpr int64_t  DEFAULT_MAX_ORDER_QUANTITY     = 1000;
constexpr int64_t  DEFAULT_MAX_POSITION           = 5000;
constexpr double   DEFAULT_MAX_DAILY_LOSS         = 5000.0;    // $5,000
constexpr double   DEFAULT_MAX_DRAWDOWN           = 10000.0;   // $10,000
constexpr int      DEFAULT_MAX_OPEN_ORDERS        = 100;
constexpr int      DEFAULT_MAX_ORDERS_PER_SECOND  = 50;

// ==============================================================================
// Fee Defaults (basis points — 1 bp = 0.01%)
// ==============================================================================
constexpr double   DEFAULT_MAKER_FEE_BPS = 1.0;   // 0.01%
constexpr double   DEFAULT_TAKER_FEE_BPS = 2.5;   // 0.025%
constexpr double   DEFAULT_SLIPPAGE_BPS  = 0.5;    // 0.005%

} // namespace hft::constants
