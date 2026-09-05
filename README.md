# Low-Latency Trading Simulator

![C++](https://img.shields.io/badge/C++-20-blue.svg)
![CMake](https://img.shields.io/badge/CMake-3.20+-green.svg)

A high-performance, deterministic C++20 limit order book and trading simulator designed for quantitative research, backtesting, and strategy validation.

> **Disclaimer:** This software is an educational/research trading simulator. It does not guarantee trading profits, and backtest results are not indicative of future performance. Real-money trading is strictly DISABLED by default.

---

## Status

- [x] **Phase 1**: Project skeleton, CMake, types, logger, CLI, tests
- [x] **Phase 2**: Order, Price Level, OrderBook, MatchingEngine, Trade representation, Baseline Benchmark

---

## Features

- **Price-Time Priority Order Book:** Price-time (FIFO) matching engine supporting Limit and Market orders.
- **Order Lifecycle Management:** Full support for order creation, partial/complete fills, cancellations, and quantity modifications.
- **Invariant Enforcement:** Strict post-matching sanity checks (non-crossed books, trade quantity conservation, active ID mapping).
- **Deterministic Market Data Replay:** Replay historical market data at variable speeds for backtesting.
- **Robust Risk Engine:** Comprehensive pre-trade risk checks (max position, order value, drawdown limits) and an independent kill switch.

---

## Architecture & Data Flow

```
Incoming Order
      ↓
MatchingEngine
  ├── Checks price crossing against resting orders
  ├── Matches FIFO at best price level
  └── Generates Trade records
      ↓
Unfilled Limit Order (if any) → OrderBook (bids_/asks_ maps + std::deque)
```

---

## Matching Rules & Invariants

1. **Price Priority**: Better price orders match first (highest bid / lowest ask).
2. **Time Priority (FIFO)**: Same price level orders match in arrival order.
3. **Execution Price**: Matches execute at the resting (passive) order's price.
4. **Book Invariants**:
   - Post-matching `best_bid < best_ask` (never crossed).
   - Order quantities are non-negative and conserved.
   - Filled/cancelled orders leave active matching structures.

---

## Baseline Performance Benchmark (Phase 2)

Tested on single-threaded Debug/Release baseline (MinGW GCC 16.2):

| Operation | Throughput |
|-----------|------------|
| Insertion | ~1.54M ops/sec |
| Cancellation | ~3.84M ops/sec |
| Matching | ~1.12M ops/sec |

*Note: This represents the initial, unoptimized baseline using standard C++ containers (`std::map`, `std::unordered_map`, `std::deque`). Optimization phases will benchmark cache-aligned, zero-allocation structures against this baseline.*

---

## Getting Started

### Prerequisites

- **Compiler:** GCC 13+ or Clang 17+ (C++20 support required)
- **Build System:** CMake 3.20+ with Ninja
- **OS:** Linux or Windows (MSYS2 / MinGW-w64 / WSL2)

### Building the Project

```bash
git clone https://github.com/gaurav8299/low-latency-trading-simulator.git
cd low-latency-trading-simulator

# Configure and build
mkdir build && cd build
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build . -j
```

### Running Tests & Benchmarks

```bash
# Run unit tests (80+ test cases)
ctest --output-on-failure

# Run baseline benchmark
./benchmarks/bench_matching
```

---

## Project Structure

```
.
├── benchmarks/             # Baseline performance benchmarks
│   └── bench_matching.cpp
├── config/
│   └── simulation.yaml     # System & risk configuration
├── include/
│   ├── common/             # Enums, types, logger
│   └── orderbook/          # Order, Trade, OrderBook, MatchingEngine headers
├── src/
│   ├── common/
│   └── orderbook/          # Core matching logic implementation
└── tests/                  # GoogleTest unit & integration suites
```
