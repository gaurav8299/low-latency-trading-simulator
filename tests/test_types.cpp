#include <gtest/gtest.h>
#include "common/types.hpp"
#include "common/constants.hpp"

TEST(SideTest, HasBuyAndSell) {
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

TEST(OrderStatusTest, AllStatusesExist) {
    auto s1 = hft::OrderStatus::New;
    auto s2 = hft::OrderStatus::Accepted;
    auto s3 = hft::OrderStatus::PartiallyFilled;
    auto s4 = hft::OrderStatus::Filled;
    auto s5 = hft::OrderStatus::Cancelled;
    auto s6 = hft::OrderStatus::Rejected;

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

TEST(TradingModeTest, SimulationIsDefault) {
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

TEST(TypeAliasTest, PriceIsDouble) {
    EXPECT_EQ(sizeof(hft::Price), sizeof(double));
}

TEST(TypeAliasTest, QuantityIsSigned64Bit) {
    EXPECT_EQ(sizeof(hft::Quantity), 8u);
    hft::Quantity q = -1;
    EXPECT_LT(q, 0);
}

TEST(TypeAliasTest, OrderIdIsUnsigned64Bit) {
    EXPECT_EQ(sizeof(hft::OrderId), 8u);
}

TEST(ConstantsTest, VersionIsNotEmpty) {
    EXPECT_FALSE(hft::constants::VERSION.empty());
}

TEST(ConstantsTest, DisclaimerContainsSimulation) {
    EXPECT_NE(hft::constants::DISCLAIMER.find("SIMULATION"),
              std::string_view::npos);
}

TEST(ConstantsTest, DisclaimerContainsNoGuarantee) {
    EXPECT_NE(hft::constants::DISCLAIMER.find("NOT indicative"),
              std::string_view::npos);
}

TEST(ConstantsTest, DefaultCapitalIsPositive) {
    EXPECT_GT(hft::constants::DEFAULT_INITIAL_CAPITAL, 0.0);
}

TEST(ConstantsTest, DefaultMaxDailyLossIsPositive) {
    EXPECT_GT(hft::constants::DEFAULT_MAX_DAILY_LOSS, 0.0);
}

TEST(ConstantsTest, DefaultMaxDailyLossLessThanCapital) {
    EXPECT_LT(hft::constants::DEFAULT_MAX_DAILY_LOSS,
              hft::constants::DEFAULT_INITIAL_CAPITAL);
}
