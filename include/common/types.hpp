#pragma once
// ==============================================================================
// types.hpp — Core Type Definitions
// ==============================================================================
//
// WHAT: Defines all the fundamental types used throughout the simulator.
// WHY:  Strong types prevent bugs. Using "enum class" instead of plain integers
//        means the compiler catches mistakes like passing a Side where an
//        OrderType is expected.
// HOW:  #include <common/types.hpp> in any file that needs these types.
//
// ==============================================================================

#include <cstdint>
#include <chrono>
#include <string>
#include <string_view>

namespace hft {

// ==============================================================================
// Trading Mode — THE MOST IMPORTANT TYPE
// ==============================================================================
// This controls whether orders go to a simulator or a real exchange.
// DEFAULT IS ALWAYS SIMULATION. Real trading is disabled.
// ==============================================================================
enum class TradingMode {
    Simulation,   // Default — all orders are simulated locally
    Paper,        // Paper trading — simulated but may connect to real data
    // Live is intentionally NOT defined here.
    // It will only be added after extensive safety infrastructure.
};

// ==============================================================================
// Side — Buy or Sell
// ==============================================================================
// Every order must have a side. There is no "unknown" side.
//
// "enum class" means you must write Side::Buy, not just Buy.
// This prevents name collisions and makes code clearer.
// ==============================================================================
enum class Side {
    Buy,
    Sell
};

// ==============================================================================
// OrderType — What kind of order
// ==============================================================================
// Limit: "I want to buy/sell at THIS price or better"
// Market: "I want to buy/sell RIGHT NOW at whatever price is available"
//
// Limit orders wait in the order book. Market orders execute immediately
// (or as much as possible against the book).
// ==============================================================================
enum class OrderType {
    Limit,
    Market
};

// ==============================================================================
// OrderStatus — Lifecycle of an order
// ==============================================================================
// An order goes through states:
//   New → (PartiallyFilled →) Filled     (happy path)
//   New → Cancelled                       (you changed your mind)
//   New → Rejected                        (risk manager said no)
//
// These are mutually exclusive states.
// ==============================================================================
enum class OrderStatus {
    New,              // Just created, not yet in the book
    Accepted,         // Accepted by the exchange/simulator
    PartiallyFilled,  // Some quantity filled, some remains
    Filled,           // Completely filled — nothing left
    Cancelled,        // You cancelled it (or it was killed)
    Rejected          // Risk manager or exchange rejected it
};

// ==============================================================================
// Type Aliases — Giving meaningful names to primitive types
// ==============================================================================
// WHY: "double" tells you nothing. "Price" tells you it's a price.
//       "uint64_t" tells you nothing. "OrderId" tells you it's an ID.
//
// These are just aliases — no runtime cost. But they make code much clearer.
//
// NOTE ON PRICE REPRESENTATION:
// Using double for prices is a simplification. In production systems,
// prices are often represented as fixed-point integers to avoid floating-point
// rounding issues (e.g., price in "ticks" or millicents). We use double here
// for simplicity and will discuss the tradeoffs in docs/orderbook.md.
// ==============================================================================
using Price    = double;
using Quantity = int64_t;     // Signed to detect negative quantity bugs
using OrderId  = uint64_t;

// High-resolution timestamp — nanosecond precision
// std::chrono is C++'s type-safe time library. It prevents unit confusion
// (you can't accidentally add seconds to nanoseconds without conversion).
using Timestamp  = std::chrono::steady_clock::time_point;
using Duration   = std::chrono::nanoseconds;
using SequenceNo = uint64_t;

// ==============================================================================
// Instrument ID — What you're trading
// ==============================================================================
// For now, just a string. Later we might use a more efficient representation.
// ==============================================================================
using InstrumentId = std::string;

// ==============================================================================
// Utility functions — Converting enums to strings (for logging)
// ==============================================================================
// constexpr means "compute at compile time if possible" — zero runtime cost.
// string_view is a non-owning reference to a string — no memory allocation.
// ==============================================================================

[[nodiscard]] constexpr std::string_view to_string(Side side) {
    switch (side) {
        case Side::Buy:  return "BUY";
        case Side::Sell: return "SELL";
    }
    return "UNKNOWN";  // Should never happen with enum class
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
