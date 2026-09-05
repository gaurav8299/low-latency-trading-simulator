#pragma once

#include "orderbook/order.hpp"
#include <map>
#include <deque>
#include <unordered_map>
#include <optional>
#include <functional>

namespace hft {

class OrderBook {
public:
    // Add a new order to the book. Returns false if OrderId already exists
    // or order has invalid price/quantity.
    bool addOrder(Order order);

    // Cancel an order by ID. Returns false if not found or already inactive.
    bool cancelOrder(OrderId id);

    // Modify remaining quantity. If increased, order loses time priority.
    // Returns false if not found or already inactive.
    bool modifyOrder(OrderId id, Quantity new_remaining_qty);

    // Best prices. Returns empty if that side has no orders.
    [[nodiscard]] std::optional<Price> bestBid() const;
    [[nodiscard]] std::optional<Price> bestAsk() const;

    // Lookup
    [[nodiscard]] const Order* getOrder(OrderId id) const;
    [[nodiscard]] bool contains(OrderId id) const;

    // Counts
    [[nodiscard]] size_t totalOrders() const { return orders_.size(); }
    [[nodiscard]] size_t bidLevels() const { return bids_.size(); }
    [[nodiscard]] size_t askLevels() const { return asks_.size(); }

    // Quantity at a specific price level
    [[nodiscard]] Quantity quantityAtPrice(Side side, Price price) const;

    // --- Methods used by MatchingEngine ---

    // Get the front (earliest) order ID at a price level. Returns 0 if empty.
    [[nodiscard]] OrderId frontOrderId(Side side, Price price) const;

    // Mutable access to an order (for MatchingEngine to call fill())
    Order* getMutableOrder(OrderId id);

    // Remove a filled order from its price level (does NOT change status)
    void removeFilledOrder(OrderId id);

private:
    // Remove an order from its price level deque. Returns true if found.
    bool removeFromLevel(Side side, Price price, OrderId id);

    // Clean up empty price levels
    void cleanEmptyLevel(Side side, Price price);

    // Bid side: highest price first (greater<> reverses default ascending order)
    std::map<Price, std::deque<OrderId>, std::greater<>> bids_;

    // Ask side: lowest price first (default ascending order)
    std::map<Price, std::deque<OrderId>> asks_;

    // All orders indexed by ID for fast lookup
    std::unordered_map<OrderId, Order> orders_;
};

} // namespace hft
