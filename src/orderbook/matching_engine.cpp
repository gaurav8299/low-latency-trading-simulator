#include "orderbook/matching_engine.hpp"
#include "common/logger.hpp"

#include <algorithm>
#include <string>

namespace hft {

std::vector<Trade> MatchingEngine::submitOrder(Order order) {
    std::vector<Trade> trades;

    if (order.quantity <= 0 || order.remaining_qty <= 0) {
        Logger::instance().warning("MatchingEngine", "Rejected invalid order");
        return trades;
    }

    if (order.side == Side::Buy) {
        trades = matchBuy(order);
    } else {
        trades = matchSell(order);
    }

    // Remaining quantity goes into the book (limit orders only)
    if (order.remaining_qty > 0 && order.type == OrderType::Limit) {
        book_.addOrder(order);
    }

    return trades;
}

std::vector<Trade> MatchingEngine::matchBuy(Order& incoming) {
    std::vector<Trade> trades;

    while (incoming.remaining_qty > 0) {
        auto best_ask = book_.bestAsk();
        if (!best_ask.has_value()) break;

        // Price check
        bool crosses = (incoming.type == OrderType::Market) ||
                       (incoming.price >= *best_ask);
        if (!crosses) break;

        // Get front order at best ask (FIFO)
        OrderId resting_id = book_.frontOrderId(Side::Sell, *best_ask);
        if (resting_id == 0) break;

        Order* resting = book_.getMutableOrder(resting_id);
        if (!resting) break;

        // Trade quantity = minimum of both remaining
        Quantity trade_qty = std::min(incoming.remaining_qty,
                                      resting->remaining_qty);

        Trade trade{
            .trade_id      = next_trade_id_++,
            .buy_order_id  = incoming.id,
            .sell_order_id = resting_id,
            .instrument    = incoming.instrument,
            .price         = resting->price,
            .quantity      = trade_qty,
            .timestamp     = std::chrono::steady_clock::now()
        };
        trades.push_back(trade);

        // Fill both sides
        incoming.fill(trade_qty);
        resting->fill(trade_qty);

        // Remove filled resting order from the book's price level
        if (resting->is_filled()) {
            book_.removeFilledOrder(resting_id);
        }
    }

    return trades;
}

std::vector<Trade> MatchingEngine::matchSell(Order& incoming) {
    std::vector<Trade> trades;

    while (incoming.remaining_qty > 0) {
        auto best_bid = book_.bestBid();
        if (!best_bid.has_value()) break;

        bool crosses = (incoming.type == OrderType::Market) ||
                       (incoming.price <= *best_bid);
        if (!crosses) break;

        OrderId resting_id = book_.frontOrderId(Side::Buy, *best_bid);
        if (resting_id == 0) break;

        Order* resting = book_.getMutableOrder(resting_id);
        if (!resting) break;

        Quantity trade_qty = std::min(incoming.remaining_qty,
                                      resting->remaining_qty);

        Trade trade{
            .trade_id      = next_trade_id_++,
            .buy_order_id  = resting_id,
            .sell_order_id = incoming.id,
            .instrument    = incoming.instrument,
            .price         = resting->price,
            .quantity      = trade_qty,
            .timestamp     = std::chrono::steady_clock::now()
        };
        trades.push_back(trade);

        incoming.fill(trade_qty);
        resting->fill(trade_qty);

        if (resting->is_filled()) {
            book_.removeFilledOrder(resting_id);
        }
    }

    return trades;
}

} // namespace hft
