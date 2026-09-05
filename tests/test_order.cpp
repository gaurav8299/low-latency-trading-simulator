#include <gtest/gtest.h>
#include "orderbook/order.hpp"

using namespace hft;

static Timestamp now() {
    return std::chrono::steady_clock::now();
}

// --- Creation ---

TEST(OrderTest, CreateLimitBuy) {
    auto order = Order::create_limit(1, "AAPL", Side::Buy, 150.0, 100, now());
    EXPECT_EQ(order.id, 1u);
    EXPECT_EQ(order.instrument, "AAPL");
    EXPECT_EQ(order.side, Side::Buy);
    EXPECT_EQ(order.type, OrderType::Limit);
    EXPECT_DOUBLE_EQ(order.price, 150.0);
    EXPECT_EQ(order.quantity, 100);
    EXPECT_EQ(order.remaining_qty, 100);
    EXPECT_EQ(order.status, OrderStatus::New);
}

TEST(OrderTest, CreateLimitSell) {
    auto order = Order::create_limit(2, "GOOG", Side::Sell, 200.5, 50, now());
    EXPECT_EQ(order.side, Side::Sell);
    EXPECT_DOUBLE_EQ(order.price, 200.5);
    EXPECT_EQ(order.quantity, 50);
    EXPECT_EQ(order.remaining_qty, 50);
}

TEST(OrderTest, CreateMarketOrder) {
    auto order = Order::create_market(3, "MSFT", Side::Buy, 200, now());
    EXPECT_EQ(order.type, OrderType::Market);
    EXPECT_DOUBLE_EQ(order.price, 0.0);
    EXPECT_EQ(order.quantity, 200);
}

// --- Status checks ---

TEST(OrderTest, NewOrderIsActive) {
    auto order = Order::create_limit(1, "AAPL", Side::Buy, 100.0, 50, now());
    EXPECT_TRUE(order.is_active());
    EXPECT_FALSE(order.is_filled());
}

TEST(OrderTest, CancelledOrderIsNotActive) {
    auto order = Order::create_limit(1, "AAPL", Side::Buy, 100.0, 50, now());
    order.status = OrderStatus::Cancelled;
    EXPECT_FALSE(order.is_active());
}

TEST(OrderTest, RejectedOrderIsNotActive) {
    auto order = Order::create_limit(1, "AAPL", Side::Buy, 100.0, 50, now());
    order.status = OrderStatus::Rejected;
    EXPECT_FALSE(order.is_active());
}

// --- Fill behavior ---

TEST(OrderTest, PartialFill) {
    auto order = Order::create_limit(1, "AAPL", Side::Buy, 100.0, 100, now());
    order.fill(30);
    EXPECT_EQ(order.remaining_qty, 70);
    EXPECT_EQ(order.status, OrderStatus::PartiallyFilled);
    EXPECT_TRUE(order.is_active());
    EXPECT_FALSE(order.is_filled());
}

TEST(OrderTest, CompleteFill) {
    auto order = Order::create_limit(1, "AAPL", Side::Buy, 100.0, 100, now());
    order.fill(100);
    EXPECT_EQ(order.remaining_qty, 0);
    EXPECT_EQ(order.status, OrderStatus::Filled);
    EXPECT_FALSE(order.is_active());
    EXPECT_TRUE(order.is_filled());
}

TEST(OrderTest, MultipleFillsToCompletion) {
    auto order = Order::create_limit(1, "AAPL", Side::Sell, 50.0, 200, now());
    order.fill(80);
    EXPECT_EQ(order.remaining_qty, 120);
    EXPECT_EQ(order.status, OrderStatus::PartiallyFilled);

    order.fill(120);
    EXPECT_EQ(order.remaining_qty, 0);
    EXPECT_EQ(order.status, OrderStatus::Filled);
}

TEST(OrderTest, OriginalQuantityNeverChanges) {
    auto order = Order::create_limit(1, "AAPL", Side::Buy, 100.0, 500, now());
    order.fill(200);
    order.fill(100);
    EXPECT_EQ(order.quantity, 500);  // original unchanged
    EXPECT_EQ(order.remaining_qty, 200);
}

TEST(OrderTest, PartiallyFilledOrderIsStillActive) {
    auto order = Order::create_limit(1, "AAPL", Side::Buy, 100.0, 100, now());
    order.fill(50);
    EXPECT_EQ(order.status, OrderStatus::PartiallyFilled);
    EXPECT_TRUE(order.is_active());
}
