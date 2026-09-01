#pragma once

#include <string_view>

namespace hft::constants {

constexpr std::string_view VERSION = "0.1.0";
constexpr std::string_view PROJECT_NAME = "Low-Latency Trading Simulator";

constexpr std::string_view DISCLAIMER = R"(
================================================================================
  SIMULATION ONLY — NO REAL ORDERS ARE BEING PLACED.

  This is an educational/research HFT-style trading simulator.
  - Backtest performance is NOT indicative of future profitability.
  - Real-money trading is strictly DISABLED by default.
================================================================================
)";

constexpr double   DEFAULT_INITIAL_CAPITAL       = 100000.0;
constexpr double   DEFAULT_MAX_ORDER_VALUE        = 10000.0;
constexpr int64_t  DEFAULT_MAX_ORDER_QUANTITY     = 1000;
constexpr int64_t  DEFAULT_MAX_POSITION           = 5000;
constexpr double   DEFAULT_MAX_DAILY_LOSS         = 5000.0;
constexpr double   DEFAULT_MAX_DRAWDOWN           = 10000.0;
constexpr int      DEFAULT_MAX_OPEN_ORDERS        = 100;
constexpr int      DEFAULT_MAX_ORDERS_PER_SECOND  = 50;

constexpr double   DEFAULT_MAKER_FEE_BPS = 1.0;
constexpr double   DEFAULT_TAKER_FEE_BPS = 2.5;
constexpr double   DEFAULT_SLIPPAGE_BPS  = 0.5;

} // namespace hft::constants
