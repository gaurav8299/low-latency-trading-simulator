# HFT-Style Trading Simulator

> **SIMULATION ONLY — NO REAL ORDERS ARE BEING PLACED.**
>
> This is an educational trading simulator for personal learning and paper trading.
> It does NOT guarantee profits. Backtest results do NOT predict future performance.
> Real-money trading is **disabled by default**.

---

## What Is This?

An educational HFT-style trading simulator that teaches you:

- How limit order books work (price-time priority matching)
- How trading strategies generate signals and orders
- How risk management protects against catastrophic losses
- How transaction costs and latency affect performance
- How to measure and benchmark trading systems

This is **NOT** institutional HFT. It is a learning tool.

---

## Quick Start

### Prerequisites

**Windows** (native build with MSVC):
```powershell
# Install Visual Studio 2022 with C++ Desktop Development workload
# Install CMake (https://cmake.org/download/) and add to PATH
# Install Git (https://git-scm.com/)

# Verify:
cmake --version   # Need 3.20+
git --version
```

**Ubuntu/WSL2**:
```bash
sudo apt update
sudo apt install -y build-essential cmake git g++-13
```

### Build

```bash
# Clone or navigate to the project
cd hft-simulator

# Create build directory
mkdir build
cd build

# Configure (Debug build with sanitizers)
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Build
cmake --build . -j
```

**Windows with MSVC** (from Developer Command Prompt or PowerShell):
```powershell
cd hft-simulator
mkdir build
cd build
cmake ..
cmake --build . --config Debug -j
```

### Run

```bash
# Linux / WSL2
./hft_simulator --mode simulation

# Windows
.\Debug\hft_simulator.exe --mode simulation

# Show help
./hft_simulator --help

# Enable verbose logging
./hft_simulator --mode simulation --verbose
```

### Run Tests

```bash
# Linux / WSL2
ctest --output-on-failure

# Windows
ctest --output-on-failure -C Debug
```

---

## Project Structure

```
hft-simulator/
├── CMakeLists.txt          # Build system configuration
├── README.md               # This file
├── LICENSE                  # MIT License
├── .gitignore              # Files Git should ignore
├── .env.example            # Template for credentials (never real values)
│
├── config/
│   └── simulation.yaml     # All tunable parameters (risk, fees, latency)
│
├── include/                # Header files (.hpp) — the "interfaces"
│   └── common/
│       ├── types.hpp       # Core types: Side, OrderType, Price, Quantity
│       ├── constants.hpp   # Version, disclaimer, default limits
│       └── logger.hpp      # Structured logging
│
├── src/                    # Source files (.cpp) — the "implementations"
│   ├── main.cpp            # Program entry point
│   └── common/
│       └── logger.cpp      # Logger implementation
│
├── tests/                  # Automated tests
│   ├── CMakeLists.txt      # Test build config
│   └── test_types.cpp      # Tests for core types
│
├── data/
│   └── sample/
│       └── sample_market_data.csv  # Synthetic test data (NOT real)
│
├── benchmarks/             # Performance tests (future)
├── python/                 # Analysis and plotting (future)
└── docs/                   # Documentation (future)
```

### What Each Folder Does

| Folder | Purpose |
|--------|---------|
| `include/` | **Header files** — declare what classes/functions exist (the "API") |
| `src/` | **Source files** — the actual code that runs |
| `tests/` | **Tests** — automated checks that the code is correct |
| `config/` | **Configuration** — parameters you can change without recompiling |
| `data/` | **Market data files** — CSV files the simulator reads |
| `benchmarks/` | **Performance tests** — how fast is the code? |
| `python/` | **Analysis scripts** — charts, metrics, research |
| `docs/` | **Documentation** — detailed explanations |

---

## Configuration

Edit `config/simulation.yaml` to change:

- **Trading mode**: `simulation` or `paper` (never `live`)
- **Capital**: Starting cash amount
- **Risk limits**: Max position, max loss, max orders, etc.
- **Fees**: Maker/taker fees in basis points
- **Latency**: Simulated delays in microseconds
- **Strategy**: Which strategy to run and its parameters

See the file for full documentation of every parameter.

---

## Safety Features

1. **Default mode is SIMULATION** — no real orders ever
2. **No live trading module exists** — it must be explicitly built
3. **No credentials in code** — use `.env` for future API keys
4. **Kill switch** — automatic shutdown on risk limit breach
5. **Every order checked** — risk engine validates before every order
6. **Disclaimer on every startup** — constant reminder this is simulation

---

## Current Status

- [x] **Phase 1**: Project skeleton, CMake, types, logger, tests
- [ ] Phase 2: Order + OrderBook
- [ ] Phase 3: MatchingEngine
- [ ] Phase 4: Market data replay
- [ ] Phase 5: Portfolio + P&L
- [ ] Phase 6: Risk engine
- [ ] Phase 7: Strategy framework
- [ ] Phase 8: Backtester
- [ ] Phase 9: Latency model
- [ ] Phase 10: Benchmarking
- [ ] Phase 11: Concurrency
- [ ] Phase 12: Python analytics
- [ ] Phase 13: Failure testing
- [ ] Phase 14: Documentation

---

## License

MIT — see [LICENSE](LICENSE)

**Trading Disclaimer**: This software is for educational purposes only. It does not guarantee trading profits. Past performance does not predict future results. Never risk money you cannot afford to lose.
