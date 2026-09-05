#include "orderbook/matching_engine.hpp"
#include <iostream>
#include <chrono>
#include <vector>
#include <iomanip>

using namespace hft;

int main() {
    std::cout << "=====================================================\n";
    std::cout << "       Phase 2 Baseline Benchmark                    \n";
    std::cout << "=====================================================\n\n";

    constexpr int NUM_OPERATIONS = 100'000;
    auto now = std::chrono::steady_clock::now();

    // -----------------------------------------------------------------
    // 1. Insertion Throughput Benchmark (Non-crossing limit orders)
    // -----------------------------------------------------------------
    {
        MatchingEngine engine;
        auto start = std::chrono::high_resolution_clock::now();

        for (int i = 1; i <= NUM_OPERATIONS; ++i) {
            // Alternating Buy @ 90.0 and Sell @ 110.0 (won't match)
            Side side = (i % 2 == 0) ? Side::Buy : Side::Sell;
            Price price = (side == Side::Buy) ? 90.0 : 110.0;
            engine.submitOrder(Order::create_limit(i, "BENCH", side, price, 10, now));
        }

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> diff = end - start;
        double ops_per_sec = NUM_OPERATIONS / diff.count();

        std::cout << "[Insertion Benchmark]\n";
        std::cout << "  Operations: " << NUM_OPERATIONS << "\n";
        std::cout << "  Total Time: " << std::fixed << std::setprecision(4) << diff.count() << " s\n";
        std::cout << "  Throughput: " << std::fixed << std::setprecision(0) << ops_per_sec << " ops/sec\n\n";
    }

    // -----------------------------------------------------------------
    // 2. Cancellation Throughput Benchmark
    // -----------------------------------------------------------------
    {
        MatchingEngine engine;
        for (int i = 1; i <= NUM_OPERATIONS; ++i) {
            Side side = (i % 2 == 0) ? Side::Buy : Side::Sell;
            Price price = (side == Side::Buy) ? 90.0 : 110.0;
            engine.submitOrder(Order::create_limit(i, "BENCH", side, price, 10, now));
        }

        auto start = std::chrono::high_resolution_clock::now();

        for (int i = 1; i <= NUM_OPERATIONS; ++i) {
            engine.cancelOrder(i);
        }

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> diff = end - start;
        double ops_per_sec = NUM_OPERATIONS / diff.count();

        std::cout << "[Cancellation Benchmark]\n";
        std::cout << "  Operations: " << NUM_OPERATIONS << "\n";
        std::cout << "  Total Time: " << std::fixed << std::setprecision(4) << diff.count() << " s\n";
        std::cout << "  Throughput: " << std::fixed << std::setprecision(0) << ops_per_sec << " ops/sec\n\n";
    }

    // -----------------------------------------------------------------
    // 3. Matching Throughput Benchmark (Fully crossing orders)
    // -----------------------------------------------------------------
    {
        MatchingEngine engine;

        // Populate sell orders first
        for (int i = 1; i <= NUM_OPERATIONS; ++i) {
            engine.submitOrder(Order::create_limit(i, "BENCH", Side::Sell, 100.0, 10, now));
        }

        auto start = std::chrono::high_resolution_clock::now();

        // Submit matching buy orders
        for (int i = NUM_OPERATIONS + 1; i <= NUM_OPERATIONS * 2; ++i) {
            engine.submitOrder(Order::create_limit(i, "BENCH", Side::Buy, 100.0, 10, now));
        }

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> diff = end - start;
        double ops_per_sec = NUM_OPERATIONS / diff.count();

        std::cout << "[Matching Benchmark]\n";
        std::cout << "  Matches:    " << NUM_OPERATIONS << "\n";
        std::cout << "  Total Time: " << std::fixed << std::setprecision(4) << diff.count() << " s\n";
        std::cout << "  Throughput: " << std::fixed << std::setprecision(0) << ops_per_sec << " ops/sec\n\n";
    }

    return 0;
}
