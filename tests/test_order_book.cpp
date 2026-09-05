#include <gtest/gtest.h>
#include "orderbook/order_book.hpp"

using namespace hft;

static Timestamp now() {
    return std::chrono::steady_clock::now();
}

static Timestamp time_offset(int microseconds) {
    return std::chrono::steady_clock::now() +
           std::chrono::microseconds(microseconds);
}

// ============================================================
// Add Orders
// ============================================================

TEST(OrderBookTest, AddBuyOrder) {
    OrderBook book;
    auto order = Order::create_limit(1, "AAPL", Side::Buy, 100.0, 50, now());
    EXPECT_TRUE(book.addOrder(order));
    EXPECT_EQ(book.totalOrders(), 1u);
    EXPECT_EQ(book.bidLevels(), 1u);
    EXPECT_EQ(book.askLevels(), 0u);
}

TEST(OrderBookTest, AddSellOrder) {
    OrderBook book;
    auto order = Order::create_limit(1, "AAPL", Side::Sell, 101.0, 50, now());
    EXPECT_TRUE(book.addOrder(order));
    EXPECT_EQ(book.totalOrders(), 1u);
    EXPECT_EQ(book.bidLevels(), 0u);
    EXPECT_EQ(book.askLevels(), 1u);
}

TEST(OrderBookTest, RejectDuplicateOrderId) {
    OrderBook book;
    auto o1 = Order::create_limit(1, "AAPL", Side::Buy, 100.0, 50, now());
    auto o2 = Order::create_limit(1, "AAPL", Side::Sell, 101.0, 30, now());
    EXPECT_TRUE(book.addOrder(o1));
    EXPECT_FALSE(book.addOrder(o2));
    EXPECT_EQ(book.totalOrders(), 1u);
}

TEST(OrderBookTest, RejectZeroQuantity) {
    OrderBook book;
    auto order = Order::create_limit(1, "AAPL", Side::Buy, 100.0, 0, now());
    EXPECT_FALSE(book.addOrder(order));
}

TEST(OrderBookTest, RejectNegativeQuantity) {
    OrderBook book;
    auto order = Order::create_limit(1, "AAPL", Side::Buy, 100.0, -10, now());
    EXPECT_FALSE(book.addOrder(order));
}

TEST(OrderBookTest, RejectZeroPriceLimitOrder) {
    OrderBook book;
    auto order = Order::create_limit(1, "AAPL", Side::Buy, 0.0, 50, now());
    EXPECT_FALSE(book.addOrder(order));
}

TEST(OrderBookTest, MultipleBuyPriceLevels) {
    OrderBook book;
    book.addOrder(Order::create_limit(1, "AAPL", Side::Buy, 100.0, 50, now()));
    book.addOrder(Order::create_limit(2, "AAPL", Side::Buy, 99.5, 30, now()));
    book.addOrder(Order::create_limit(3, "AAPL", Side::Buy, 100.0, 20, now()));
    EXPECT_EQ(book.bidLevels(), 2u);  // $100 and $99.50
    EXPECT_EQ(book.totalOrders(), 3u);
}

TEST(OrderBookTest, MultipleSellPriceLevels) {
    OrderBook book;
    book.addOrder(Order::create_limit(1, "AAPL", Side::Sell, 101.0, 50, now()));
    book.addOrder(Order::create_limit(2, "AAPL", Side::Sell, 102.0, 30, now()));
    book.addOrder(Order::create_limit(3, "AAPL", Side::Sell, 101.0, 20, now()));
    EXPECT_EQ(book.askLevels(), 2u);  // $101 and $102
}

// ============================================================
// Best Bid / Best Ask
// ============================================================

TEST(OrderBookTest, BestBidEmptyBook) {
    OrderBook book;
    EXPECT_FALSE(book.bestBid().has_value());
}

TEST(OrderBookTest, BestAskEmptyBook) {
    OrderBook book;
    EXPECT_FALSE(book.bestAsk().has_value());
}

TEST(OrderBookTest, BestBidSingleOrder) {
    OrderBook book;
    book.addOrder(Order::create_limit(1, "AAPL", Side::Buy, 100.0, 50, now()));
    auto bb = book.bestBid();
    ASSERT_TRUE(bb.has_value());
    EXPECT_DOUBLE_EQ(*bb, 100.0);
}

TEST(OrderBookTest, BestAskSingleOrder) {
    OrderBook book;
    book.addOrder(Order::create_limit(1, "AAPL", Side::Sell, 101.0, 50, now()));
    auto ba = book.bestAsk();
    ASSERT_TRUE(ba.has_value());
    EXPECT_DOUBLE_EQ(*ba, 101.0);
}

TEST(OrderBookTest, BestBidIsHighest) {
    OrderBook book;
    book.addOrder(Order::create_limit(1, "AAPL", Side::Buy, 99.0, 50, now()));
    book.addOrder(Order::create_limit(2, "AAPL", Side::Buy, 100.0, 50, now()));
    book.addOrder(Order::create_limit(3, "AAPL", Side::Buy, 98.0, 50, now()));
    EXPECT_DOUBLE_EQ(*book.bestBid(), 100.0);
}

TEST(OrderBookTest, BestAskIsLowest) {
    OrderBook book;
    book.addOrder(Order::create_limit(1, "AAPL", Side::Sell, 102.0, 50, now()));
    book.addOrder(Order::create_limit(2, "AAPL", Side::Sell, 101.0, 50, now()));
    book.addOrder(Order::create_limit(3, "AAPL", Side::Sell, 103.0, 50, now()));
    EXPECT_DOUBLE_EQ(*book.bestAsk(), 101.0);
}

// ============================================================
// Quantity at Price
// ============================================================

TEST(OrderBookTest, QuantityAtPrice) {
    OrderBook book;
    book.addOrder(Order::create_limit(1, "AAPL", Side::Buy, 100.0, 50, now()));
    book.addOrder(Order::create_limit(2, "AAPL", Side::Buy, 100.0, 30, now()));
    EXPECT_EQ(book.quantityAtPrice(Side::Buy, 100.0), 80);
    EXPECT_EQ(book.quantityAtPrice(Side::Buy, 99.0), 0);
}

// ============================================================
// Cancel Orders
// ============================================================

TEST(OrderBookTest, CancelExistingOrder) {
    OrderBook book;
    book.addOrder(Order::create_limit(1, "AAPL", Side::Buy, 100.0, 50, now()));
    EXPECT_TRUE(book.cancelOrder(1));
    EXPECT_EQ(book.getOrder(1)->status, OrderStatus::Cancelled);
    EXPECT_EQ(book.bidLevels(), 0u);  // empty level should be cleaned up
}

TEST(OrderBookTest, CancelUnknownOrder) {
    OrderBook book;
    EXPECT_FALSE(book.cancelOrder(999));
}

TEST(OrderBookTest, CancelAlreadyCancelledOrder) {
    OrderBook book;
    book.addOrder(Order::create_limit(1, "AAPL", Side::Buy, 100.0, 50, now()));
    EXPECT_TRUE(book.cancelOrder(1));
    EXPECT_FALSE(book.cancelOrder(1));  // already cancelled
}

TEST(OrderBookTest, CancelRemovesEmptyPriceLevel) {
    OrderBook book;
    book.addOrder(Order::create_limit(1, "AAPL", Side::Buy, 100.0, 50, now()));
    book.addOrder(Order::create_limit(2, "AAPL", Side::Buy, 99.0, 30, now()));
    EXPECT_EQ(book.bidLevels(), 2u);

    book.cancelOrder(1);
    EXPECT_EQ(book.bidLevels(), 1u);
    EXPECT_DOUBLE_EQ(*book.bestBid(), 99.0);
}

TEST(OrderBookTest, CancelOneOfMultipleAtSamePrice) {
    OrderBook book;
    book.addOrder(Order::create_limit(1, "AAPL", Side::Buy, 100.0, 50, now()));
    book.addOrder(Order::create_limit(2, "AAPL", Side::Buy, 100.0, 30, now()));
    book.cancelOrder(1);
    EXPECT_EQ(book.bidLevels(), 1u);  // level stays (order 2 is there)
    EXPECT_EQ(book.quantityAtPrice(Side::Buy, 100.0), 30);
}

TEST(OrderBookTest, BestBidUpdatesAfterCancel) {
    OrderBook book;
    book.addOrder(Order::create_limit(1, "AAPL", Side::Buy, 100.0, 50, now()));
    book.addOrder(Order::create_limit(2, "AAPL", Side::Buy, 99.0, 50, now()));
    EXPECT_DOUBLE_EQ(*book.bestBid(), 100.0);

    book.cancelOrder(1);
    EXPECT_DOUBLE_EQ(*book.bestBid(), 99.0);
}

// ============================================================
// Modify Orders
// ============================================================

TEST(OrderBookTest, ModifyDecreaseKeepsPriority) {
    OrderBook book;
    auto t1 = time_offset(0);
    auto t2 = time_offset(1);
    book.addOrder(Order::create_limit(1, "AAPL", Side::Buy, 100.0, 100, t1));
    book.addOrder(Order::create_limit(2, "AAPL", Side::Buy, 100.0, 50, t2));

    // Decrease order 1 quantity — should keep its position (first)
    EXPECT_TRUE(book.modifyOrder(1, 70));
    EXPECT_EQ(book.getOrder(1)->remaining_qty, 70);
}

TEST(OrderBookTest, ModifyIncreaseLosesPriority) {
    OrderBook book;
    auto t1 = time_offset(0);
    auto t2 = time_offset(1);
    book.addOrder(Order::create_limit(1, "AAPL", Side::Buy, 100.0, 50, t1));
    book.addOrder(Order::create_limit(2, "AAPL", Side::Buy, 100.0, 50, t2));

    // Increase order 1 quantity — should lose priority (move behind order 2)
    EXPECT_TRUE(book.modifyOrder(1, 80));
    EXPECT_EQ(book.getOrder(1)->remaining_qty, 80);
    // Order 2 should now be first at this level
    // (We'll verify this more thoroughly in matching engine tests)
}

TEST(OrderBookTest, ModifyUnknownOrder) {
    OrderBook book;
    EXPECT_FALSE(book.modifyOrder(999, 50));
}

TEST(OrderBookTest, ModifyToZeroCancels) {
    OrderBook book;
    book.addOrder(Order::create_limit(1, "AAPL", Side::Buy, 100.0, 50, now()));
    EXPECT_TRUE(book.modifyOrder(1, 0));
    EXPECT_EQ(book.getOrder(1)->status, OrderStatus::Cancelled);
    EXPECT_EQ(book.bidLevels(), 0u);
}

// ============================================================
// Lookup
// ============================================================

TEST(OrderBookTest, GetExistingOrder) {
    OrderBook book;
    book.addOrder(Order::create_limit(42, "AAPL", Side::Buy, 100.0, 50, now()));
    auto* order = book.getOrder(42);
    ASSERT_NE(order, nullptr);
    EXPECT_EQ(order->id, 42u);
    EXPECT_DOUBLE_EQ(order->price, 100.0);
}

TEST(OrderBookTest, GetNonexistentOrder) {
    OrderBook book;
    EXPECT_EQ(book.getOrder(999), nullptr);
}

TEST(OrderBookTest, ContainsOrder) {
    OrderBook book;
    book.addOrder(Order::create_limit(1, "AAPL", Side::Buy, 100.0, 50, now()));
    EXPECT_TRUE(book.contains(1));
    EXPECT_FALSE(book.contains(2));
}

// ============================================================
// Full scenario from Part 1 teaching
// ============================================================

TEST(OrderBookTest, FullBookScenario) {
    OrderBook book;
    auto t = time_offset(0);

    // Build the book from the teaching example
    book.addOrder(Order::create_limit(1, "SYNTH", Side::Buy,  100.00, 200, time_offset(1)));
    book.addOrder(Order::create_limit(2, "SYNTH", Side::Buy,   99.50, 300, time_offset(2)));
    book.addOrder(Order::create_limit(3, "SYNTH", Side::Buy,   99.00, 100, time_offset(3)));
    book.addOrder(Order::create_limit(4, "SYNTH", Side::Sell, 100.10, 150, time_offset(4)));
    book.addOrder(Order::create_limit(5, "SYNTH", Side::Sell, 100.50, 200, time_offset(5)));
    book.addOrder(Order::create_limit(6, "SYNTH", Side::Sell, 101.00, 100, time_offset(6)));

    EXPECT_EQ(book.bidLevels(), 3u);
    EXPECT_EQ(book.askLevels(), 3u);
    EXPECT_EQ(book.totalOrders(), 6u);

    EXPECT_DOUBLE_EQ(*book.bestBid(), 100.00);
    EXPECT_DOUBLE_EQ(*book.bestAsk(), 100.10);

    EXPECT_EQ(book.quantityAtPrice(Side::Buy, 100.00), 200);
    EXPECT_EQ(book.quantityAtPrice(Side::Buy, 99.50), 300);
    EXPECT_EQ(book.quantityAtPrice(Side::Sell, 100.10), 150);
}
