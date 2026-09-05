#include <gtest/gtest.h>
#include "orderbook/matching_engine.hpp"

using namespace hft;

static Timestamp time_at(int us) {
    return std::chrono::steady_clock::now() + std::chrono::microseconds(us);
}

// ============================================================
// No Match scenarios
// ============================================================

TEST(MatchingEngineTest, BuyIntoEmptyBook) {
    MatchingEngine engine;
    auto order = Order::create_limit(1, "AAPL", Side::Buy, 100.0, 50, time_at(0));
    auto trades = engine.submitOrder(order);
    EXPECT_TRUE(trades.empty());
    EXPECT_DOUBLE_EQ(*engine.book().bestBid(), 100.0);
}

TEST(MatchingEngineTest, SellIntoEmptyBook) {
    MatchingEngine engine;
    auto order = Order::create_limit(1, "AAPL", Side::Sell, 100.0, 50, time_at(0));
    auto trades = engine.submitOrder(order);
    EXPECT_TRUE(trades.empty());
    EXPECT_DOUBLE_EQ(*engine.book().bestAsk(), 100.0);
}

TEST(MatchingEngineTest, NoCrossingNoMatch) {
    MatchingEngine engine;
    engine.submitOrder(Order::create_limit(1, "AAPL", Side::Buy, 99.0, 100, time_at(0)));
    auto trades = engine.submitOrder(
        Order::create_limit(2, "AAPL", Side::Sell, 100.0, 100, time_at(1)));
    EXPECT_TRUE(trades.empty());
    EXPECT_EQ(engine.book().bidLevels(), 1u);
    EXPECT_EQ(engine.book().askLevels(), 1u);
}

// ============================================================
// Exact match
// ============================================================

TEST(MatchingEngineTest, ExactMatch) {
    MatchingEngine engine;
    engine.submitOrder(Order::create_limit(1, "AAPL", Side::Buy, 100.0, 100, time_at(0)));
    auto trades = engine.submitOrder(
        Order::create_limit(2, "AAPL", Side::Sell, 100.0, 100, time_at(1)));

    ASSERT_EQ(trades.size(), 1u);
    EXPECT_EQ(trades[0].buy_order_id, 1u);
    EXPECT_EQ(trades[0].sell_order_id, 2u);
    EXPECT_EQ(trades[0].quantity, 100);
    EXPECT_DOUBLE_EQ(trades[0].price, 100.0);

    // Book should be empty after exact match
    EXPECT_EQ(engine.book().bidLevels(), 0u);
    EXPECT_EQ(engine.book().askLevels(), 0u);
}

TEST(MatchingEngineTest, ExactMatchSellFirst) {
    MatchingEngine engine;
    engine.submitOrder(Order::create_limit(1, "AAPL", Side::Sell, 100.0, 100, time_at(0)));
    auto trades = engine.submitOrder(
        Order::create_limit(2, "AAPL", Side::Buy, 100.0, 100, time_at(1)));

    ASSERT_EQ(trades.size(), 1u);
    EXPECT_EQ(trades[0].buy_order_id, 2u);
    EXPECT_EQ(trades[0].sell_order_id, 1u);
    EXPECT_EQ(trades[0].quantity, 100);
}

// ============================================================
// Partial fills
// ============================================================

TEST(MatchingEngineTest, IncomingPartiallyFilled) {
    MatchingEngine engine;
    // Resting: SELL 50
    engine.submitOrder(Order::create_limit(1, "AAPL", Side::Sell, 100.0, 50, time_at(0)));
    // Incoming: BUY 100 — only 50 can match
    auto trades = engine.submitOrder(
        Order::create_limit(2, "AAPL", Side::Buy, 100.0, 100, time_at(1)));

    ASSERT_EQ(trades.size(), 1u);
    EXPECT_EQ(trades[0].quantity, 50);

    // Remaining 50 of the buy should be in the book
    EXPECT_DOUBLE_EQ(*engine.book().bestBid(), 100.0);
    EXPECT_EQ(engine.book().quantityAtPrice(Side::Buy, 100.0), 50);
    // Ask side empty
    EXPECT_FALSE(engine.book().bestAsk().has_value());
}

TEST(MatchingEngineTest, RestingPartiallyFilled) {
    MatchingEngine engine;
    // Resting: BUY 200
    engine.submitOrder(Order::create_limit(1, "AAPL", Side::Buy, 100.0, 200, time_at(0)));
    // Incoming: SELL 80
    auto trades = engine.submitOrder(
        Order::create_limit(2, "AAPL", Side::Sell, 100.0, 80, time_at(1)));

    ASSERT_EQ(trades.size(), 1u);
    EXPECT_EQ(trades[0].quantity, 80);

    // Resting order should have 120 remaining
    EXPECT_EQ(engine.book().quantityAtPrice(Side::Buy, 100.0), 120);
    EXPECT_FALSE(engine.book().bestAsk().has_value());
}

// ============================================================
// Multiple price levels
// ============================================================

TEST(MatchingEngineTest, MatchAcrossMultipleAskLevels) {
    MatchingEngine engine;
    engine.submitOrder(Order::create_limit(1, "AAPL", Side::Sell, 100.0, 50, time_at(0)));
    engine.submitOrder(Order::create_limit(2, "AAPL", Side::Sell, 100.5, 50, time_at(1)));
    engine.submitOrder(Order::create_limit(3, "AAPL", Side::Sell, 101.0, 50, time_at(2)));

    // BUY 120 @ $101 — should sweep through $100 and $100.50
    auto trades = engine.submitOrder(
        Order::create_limit(4, "AAPL", Side::Buy, 101.0, 120, time_at(3)));

    ASSERT_EQ(trades.size(), 3u);

    // First trade at $100 for 50
    EXPECT_DOUBLE_EQ(trades[0].price, 100.0);
    EXPECT_EQ(trades[0].quantity, 50);

    // Second trade at $100.50 for 50
    EXPECT_DOUBLE_EQ(trades[1].price, 100.5);
    EXPECT_EQ(trades[1].quantity, 50);

    // Third trade at $101 for 20
    EXPECT_DOUBLE_EQ(trades[2].price, 101.0);
    EXPECT_EQ(trades[2].quantity, 20);

    // $101 level should have 30 remaining
    EXPECT_EQ(engine.book().quantityAtPrice(Side::Sell, 101.0), 30);
}

TEST(MatchingEngineTest, MatchAcrossMultipleBidLevels) {
    MatchingEngine engine;
    engine.submitOrder(Order::create_limit(1, "AAPL", Side::Buy, 101.0, 50, time_at(0)));
    engine.submitOrder(Order::create_limit(2, "AAPL", Side::Buy, 100.5, 50, time_at(1)));
    engine.submitOrder(Order::create_limit(3, "AAPL", Side::Buy, 100.0, 50, time_at(2)));

    // SELL 80 @ $100 — should match $101 (50) then $100.50 (30)
    auto trades = engine.submitOrder(
        Order::create_limit(4, "AAPL", Side::Sell, 100.0, 80, time_at(3)));

    ASSERT_EQ(trades.size(), 2u);
    EXPECT_DOUBLE_EQ(trades[0].price, 101.0);
    EXPECT_EQ(trades[0].quantity, 50);
    EXPECT_DOUBLE_EQ(trades[1].price, 100.5);
    EXPECT_EQ(trades[1].quantity, 30);
}

// ============================================================
// Price-time priority (FIFO)
// ============================================================

TEST(MatchingEngineTest, FIFOAtSamePrice) {
    MatchingEngine engine;
    // Two sells at same price, order A arrives first
    engine.submitOrder(Order::create_limit(1, "AAPL", Side::Sell, 100.0, 50, time_at(0)));
    engine.submitOrder(Order::create_limit(2, "AAPL", Side::Sell, 100.0, 50, time_at(1)));

    // BUY 70 — should fill order 1 first (50), then order 2 (20)
    auto trades = engine.submitOrder(
        Order::create_limit(3, "AAPL", Side::Buy, 100.0, 70, time_at(2)));

    ASSERT_EQ(trades.size(), 2u);
    EXPECT_EQ(trades[0].sell_order_id, 1u);  // Order 1 first (FIFO)
    EXPECT_EQ(trades[0].quantity, 50);
    EXPECT_EQ(trades[1].sell_order_id, 2u);  // Order 2 second
    EXPECT_EQ(trades[1].quantity, 20);
}

TEST(MatchingEngineTest, PricePriorityBeforeTimePriority) {
    MatchingEngine engine;
    // Order A: SELL @ $101 (arrived first)
    engine.submitOrder(Order::create_limit(1, "AAPL", Side::Sell, 101.0, 100, time_at(0)));
    // Order B: SELL @ $100 (arrived second but BETTER price)
    engine.submitOrder(Order::create_limit(2, "AAPL", Side::Sell, 100.0, 100, time_at(1)));

    // BUY @ $101 for 50 — should match Order B first (better price)
    auto trades = engine.submitOrder(
        Order::create_limit(3, "AAPL", Side::Buy, 101.0, 50, time_at(2)));

    ASSERT_EQ(trades.size(), 1u);
    EXPECT_EQ(trades[0].sell_order_id, 2u);  // Order B matched first
    EXPECT_DOUBLE_EQ(trades[0].price, 100.0);
}

// ============================================================
// Trade at resting order's price
// ============================================================

TEST(MatchingEngineTest, TradeAtRestingPrice) {
    MatchingEngine engine;
    // Resting SELL @ $99.50
    engine.submitOrder(Order::create_limit(1, "AAPL", Side::Sell, 99.5, 100, time_at(0)));
    // Incoming BUY @ $100 — crosses, trade at $99.50 (resting price)
    auto trades = engine.submitOrder(
        Order::create_limit(2, "AAPL", Side::Buy, 100.0, 100, time_at(1)));

    ASSERT_EQ(trades.size(), 1u);
    EXPECT_DOUBLE_EQ(trades[0].price, 99.5);  // resting order's price
}

// ============================================================
// Market orders
// ============================================================

TEST(MatchingEngineTest, MarketBuyMatchesAtAnyPrice) {
    MatchingEngine engine;
    engine.submitOrder(Order::create_limit(1, "AAPL", Side::Sell, 150.0, 100, time_at(0)));

    auto trades = engine.submitOrder(
        Order::create_market(2, "AAPL", Side::Buy, 50, time_at(1)));

    ASSERT_EQ(trades.size(), 1u);
    EXPECT_EQ(trades[0].quantity, 50);
    EXPECT_DOUBLE_EQ(trades[0].price, 150.0);
}

TEST(MatchingEngineTest, MarketOrderNoRemainder) {
    MatchingEngine engine;
    engine.submitOrder(Order::create_limit(1, "AAPL", Side::Sell, 100.0, 30, time_at(0)));

    // Market buy for 100, only 30 available — no remainder added to book
    auto trades = engine.submitOrder(
        Order::create_market(2, "AAPL", Side::Buy, 100, time_at(1)));

    ASSERT_EQ(trades.size(), 1u);
    EXPECT_EQ(trades[0].quantity, 30);
    // Market orders don't rest in the book
    EXPECT_FALSE(engine.book().bestBid().has_value());
}

// ============================================================
// Invariants
// ============================================================

TEST(MatchingEngineTest, BookNeverCrossedAfterMatch) {
    MatchingEngine engine;
    engine.submitOrder(Order::create_limit(1, "AAPL", Side::Buy, 100.0, 200, time_at(0)));
    engine.submitOrder(Order::create_limit(2, "AAPL", Side::Sell, 99.0, 50, time_at(1)));

    auto bb = engine.book().bestBid();
    auto ba = engine.book().bestAsk();

    // After matching, if both sides exist, bid < ask
    if (bb.has_value() && ba.has_value()) {
        EXPECT_LT(*bb, *ba);
    }
}

TEST(MatchingEngineTest, TradeQuantityConservation) {
    MatchingEngine engine;
    engine.submitOrder(Order::create_limit(1, "AAPL", Side::Buy, 100.0, 200, time_at(0)));
    auto trades = engine.submitOrder(
        Order::create_limit(2, "AAPL", Side::Sell, 99.0, 150, time_at(1)));

    Quantity total_traded = 0;
    for (const auto& t : trades) {
        total_traded += t.quantity;
        EXPECT_GT(t.quantity, 0);  // no zero-quantity trades
    }
    EXPECT_EQ(total_traded, 150);  // limited by smaller order
}

TEST(MatchingEngineTest, TradeIdIncrementsCorrectly) {
    MatchingEngine engine;
    engine.submitOrder(Order::create_limit(1, "AAPL", Side::Sell, 100.0, 50, time_at(0)));
    engine.submitOrder(Order::create_limit(2, "AAPL", Side::Sell, 100.5, 50, time_at(1)));

    auto trades = engine.submitOrder(
        Order::create_limit(3, "AAPL", Side::Buy, 101.0, 100, time_at(2)));

    ASSERT_EQ(trades.size(), 2u);
    EXPECT_EQ(trades[0].trade_id, 1u);
    EXPECT_EQ(trades[1].trade_id, 2u);
    EXPECT_EQ(engine.totalTradesGenerated(), 2u);
}

// ============================================================
// Complete book empty after matching
// ============================================================

TEST(MatchingEngineTest, BookEmptyAfterFullMatch) {
    MatchingEngine engine;
    engine.submitOrder(Order::create_limit(1, "AAPL", Side::Buy, 100.0, 100, time_at(0)));
    engine.submitOrder(Order::create_limit(2, "AAPL", Side::Sell, 100.0, 100, time_at(1)));

    EXPECT_EQ(engine.book().bidLevels(), 0u);
    EXPECT_EQ(engine.book().askLevels(), 0u);
    EXPECT_FALSE(engine.book().bestBid().has_value());
    EXPECT_FALSE(engine.book().bestAsk().has_value());
}

// ============================================================
// Cancel through matching engine
// ============================================================

TEST(MatchingEngineTest, CancelOrderBeforeMatch) {
    MatchingEngine engine;
    engine.submitOrder(Order::create_limit(1, "AAPL", Side::Buy, 100.0, 100, time_at(0)));
    EXPECT_TRUE(engine.cancelOrder(1));

    // Sell should not match cancelled order
    auto trades = engine.submitOrder(
        Order::create_limit(2, "AAPL", Side::Sell, 100.0, 50, time_at(1)));
    EXPECT_TRUE(trades.empty());
}
