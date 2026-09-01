#pragma once

#include <cstdint>
#include <chrono>
#include <string>
#include <string_view>

namespace hft {

enum class TradingMode {
    Simulation,
    Paper
};

enum class Side {
    Buy,
    Sell
};

enum class OrderType {
    Limit,
    Market
};

enum class OrderStatus {
    New,
    Accepted,
    PartiallyFilled,
    Filled,
    Cancelled,
    Rejected
};

using Price    = double;
using Quantity = int64_t;
using OrderId  = uint64_t;

using Timestamp  = std::chrono::steady_clock::time_point;
using Duration   = std::chrono::nanoseconds;
using SequenceNo = uint64_t;

using InstrumentId = std::string;

[[nodiscard]] constexpr std::string_view to_string(Side side) {
    switch (side) {
        case Side::Buy:  return "BUY";
        case Side::Sell: return "SELL";
    }
    return "UNKNOWN";
}

[[nodiscard]] constexpr std::string_view to_string(OrderType type) {
    switch (type) {
        case OrderType::Limit:  return "LIMIT";
        case OrderType::Market: return "MARKET";
    }
    return "UNKNOWN";
}

[[nodiscard]] constexpr std::string_view to_string(OrderStatus status) {
    switch (status) {
        case OrderStatus::New:             return "NEW";
        case OrderStatus::Accepted:        return "ACCEPTED";
        case OrderStatus::PartiallyFilled: return "PARTIALLY_FILLED";
        case OrderStatus::Filled:          return "FILLED";
        case OrderStatus::Cancelled:       return "CANCELLED";
        case OrderStatus::Rejected:        return "REJECTED";
    }
    return "UNKNOWN";
}

[[nodiscard]] constexpr std::string_view to_string(TradingMode mode) {
    switch (mode) {
        case TradingMode::Simulation: return "SIMULATION";
        case TradingMode::Paper:      return "PAPER";
    }
    return "UNKNOWN";
}

} // namespace hft
