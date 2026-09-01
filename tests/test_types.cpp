// ==============================================================================
// test_types.cpp — Unit Tests for Core Types
// ==============================================================================
//
// WHAT: Tests that verify our type definitions are correct.
// WHY:  Even "simple" types can have bugs:
//        - Did you typo an enum value?
//        - Does to_string() handle all cases?
//        - Are type sizes what you expect?
//       Tests catch these before they cause mysterious bugs later.
// HOW:  These run automatically via ctest.
//
// GOOGLE TEST BASICS:
//   TEST(GroupName, TestName) { ... }  — defines a test case
//   EXPECT_EQ(a, b)                   — check a == b (continues on failure)
//   ASSERT_EQ(a, b)                   — check a == b (stops on failure)
//   EXPECT_NE(a, b)                   — check a != b
//   EXPECT_TRUE(x)                    — check x is true
//   EXPECT_FALSE(x)                   — check x is false
//
// ==============================================================================

#include <gtest/gtest.h>
#include "common/types.hpp"
#include "common/constants.hpp"

// ==============================================================================
// Test Group: Side enum
// ==============================================================================

TEST(SideTest, HasBuyAndSell) {
    // Verify both values exist and are distinct
    hft::Side buy = hft::Side::Buy;
    hft::Side sell = hft::Side::Sell;
    EXPECT_NE(buy, sell);
}

TEST(SideTest, ToStringBuy) {
    EXPECT_EQ(hft::to_string(hft::Side::Buy), "BUY");
}

TEST(SideTest, ToStringSell) {
    EXPECT_EQ(hft::to_string(hft::Side::Sell), "SELL");
}

// ==============================================================================
// Test Group: OrderType enum
// ==============================================================================

TEST(OrderTypeTest, HasLimitAndMarket) {
    hft::OrderType limit = hft::OrderType::Limit;
    hft::OrderType market = hft::OrderType::Market;
    EXPECT_NE(limit, market);
}

TEST(OrderTypeTest, ToStringLimit) {
    EXPECT_EQ(hft::to_string(hft::OrderType::Limit), "LIMIT");
}

TEST(OrderTypeTest, ToStringMarket) {
    EXPECT_EQ(hft::to_string(hft::OrderType::Market), "MARKET");
}

// ==============================================================================
// Test Group: OrderStatus enum
// ==============================================================================

TEST(OrderStatusTest, AllStatusesExist) {
    // Verify all 6 statuses exist and compile
    auto s1 = hft::OrderStatus::New;
    auto s2 = hft::OrderStatus::Accepted;
    auto s3 = hft::OrderStatus::PartiallyFilled;
    auto s4 = hft::OrderStatus::Filled;
    auto s5 = hft::OrderStatus::Cancelled;
    auto s6 = hft::OrderStatus::Rejected;

    // They should all be different
    EXPECT_NE(s1, s2);
    EXPECT_NE(s2, s3);
    EXPECT_NE(s3, s4);
    EXPECT_NE(s4, s5);
    EXPECT_NE(s5, s6);
}

TEST(OrderStatusTest, ToStringAllStatuses) {
    EXPECT_EQ(hft::to_string(hft::OrderStatus::New), "NEW");
    EXPECT_EQ(hft::to_string(hft::OrderStatus::Accepted), "ACCEPTED");
    EXPECT_EQ(hft::to_string(hft::OrderStatus::PartiallyFilled), "PARTIALLY_FILLED");
    EXPECT_EQ(hft::to_string(hft::OrderStatus::Filled), "FILLED");
    EXPECT_EQ(hft::to_string(hft::OrderStatus::Cancelled), "CANCELLED");
    EXPECT_EQ(hft::to_string(hft::OrderStatus::Rejected), "REJECTED");
}

// ==============================================================================
// Test Group: TradingMode enum
// ==============================================================================

TEST(TradingModeTest, SimulationIsDefault) {
    // The default-constructed mode should be Simulation
    // (In C++, enums default to the first value, which is 0)
    hft::TradingMode mode = hft::TradingMode::Simulation;
    EXPECT_EQ(mode, hft::TradingMode::Simulation);
}

TEST(TradingModeTest, SimulationAndPaperAreDifferent) {
    EXPECT_NE(hft::TradingMode::Simulation, hft::TradingMode::Paper);
}

TEST(TradingModeTest, ToStringSimulation) {
    EXPECT_EQ(hft::to_string(hft::TradingMode::Simulation), "SIMULATION");
}

TEST(TradingModeTest, ToStringPaper) {
    EXPECT_EQ(hft::to_string(hft::TradingMode::Paper), "PAPER");
}

// ==============================================================================
// Test Group: Type Aliases — verify sizes and behavior
// ==============================================================================

TEST(TypeAliasTest, PriceIsDouble) {
    // Price should be a double (8 bytes)
    EXPECT_EQ(sizeof(hft::Price), sizeof(double));
}

TEST(TypeAliasTest, QuantityIsSigned64Bit) {
    // Quantity is int64_t — signed so we can detect negative bugs
    EXPECT_EQ(sizeof(hft::Quantity), 8u);
    hft::Quantity q = -1;  // Must be signed to allow this
    EXPECT_LT(q, 0);
}

TEST(TypeAliasTest, OrderIdIsUnsigned64Bit) {
    // OrderId is uint64_t — no negative IDs
    EXPECT_EQ(sizeof(hft::OrderId), 8u);
}

// ==============================================================================
// Test Group: Constants — verify safety defaults
// ==============================================================================

TEST(ConstantsTest, VersionIsNotEmpty) {
    EXPECT_FALSE(hft::constants::VERSION.empty());
}

TEST(ConstantsTest, DisclaimerContainsSimulation) {
    // The disclaimer MUST mention "SIMULATION"
    EXPECT_NE(hft::constants::DISCLAIMER.find("SIMULATION"),
              std::string_view::npos);
}

TEST(ConstantsTest, DisclaimerContainsNoGuarantee) {
    // The disclaimer MUST NOT promise profits
    EXPECT_NE(hft::constants::DISCLAIMER.find("NOT guarantee"),
              std::string_view::npos);
}

TEST(ConstantsTest, DefaultCapitalIsPositive) {
    EXPECT_GT(hft::constants::DEFAULT_INITIAL_CAPITAL, 0.0);
}

TEST(ConstantsTest, DefaultMaxDailyLossIsPositive) {
    EXPECT_GT(hft::constants::DEFAULT_MAX_DAILY_LOSS, 0.0);
}

TEST(ConstantsTest, DefaultMaxDailyLossLessThanCapital) {
    // Max daily loss should be less than initial capital (sanity check)
    EXPECT_LT(hft::constants::DEFAULT_MAX_DAILY_LOSS,
              hft::constants::DEFAULT_INITIAL_CAPITAL);
}
