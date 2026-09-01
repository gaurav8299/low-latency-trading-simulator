# Low-Latency Trading Simulator

![C++](https://img.shields.io/badge/C++-20-blue.svg)
![CMake](https://img.shields.io/badge/CMake-3.20+-green.svg)
![License](https://img.shields.io/badge/License-MIT-yellow.svg)

A high-performance, deterministic C++20 limit order book and trading simulator designed for quantitative research, backtesting, and strategy validation.

> **Disclaimer:** This software is an educational/research trading simulator. It does not guarantee trading profits, and backtest results are not indicative of future performance. Real-money trading is strictly disabled by default.

## Features

- **High-Performance Order Book:** Price-time priority limit order book.
- **Deterministic Market Data Replay:** Replay historical market data at variable speeds for backtesting.
- **Robust Risk Engine:** Comprehensive pre-trade risk checks (max position, order value, drawdown limits) and an independent kill switch.
- **Transaction Cost & Latency Modeling:** Simulates broker/exchange fees, slippage, and various network/processing latencies.
- **Modular Strategy Framework:** Plugin-style architecture for easy implementation of proprietary trading strategies.

## Architecture

The system is built with a highly decoupled, modular architecture prioritizing low latency and deterministic execution.

```
Data Ingestion (CSV/PCAP) -> Data Validator -> Market Data Replay
                                                    |
                                                    v
[ Strategy Engine ] <==> [ Risk Manager ] <==> [ Order Manager ] <==> [ Simulated Exchange / Order Book ]
                                                    |
                                                    v
                                      [ Portfolio & PnL Tracking ]
```

## Getting Started

### Prerequisites

- **Compiler:** GCC 13+ or Clang 17+ (C++20 support required)
- **Build System:** CMake 3.20+
- **OS:** Linux (Ubuntu/Debian) or Windows (via MSYS2/MinGW-w64 or WSL2)

### Building the Project

```bash
git clone https://github.com/gaurav8299/low-latency-trading-simulator.git
cd low-latency-trading-simulator

# Configure and build
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . -j$(nproc)
```

### Running the Simulator

Run the simulator using the built-in CLI options:

```bash
./build/hft_simulator --mode simulation --config ../config/simulation.yaml
```

**Options:**
- `--mode <mode>` : Trading mode (`simulation` or `paper`).
- `--config <path>` : Path to the configuration YAML file.
- `--verbose` : Enable debug-level logging.

### Running Tests

The project uses GoogleTest for unit and integration testing.

```bash
cd build
ctest --output-on-failure
```

## Configuration

System limits, strategy parameters, and fees are defined in `config/simulation.yaml`. See the file for detailed parameter documentation.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
