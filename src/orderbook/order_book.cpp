#include "orderbook/order_book.hpp"
#include "common/logger.hpp"

#include <algorithm>
#include <string>

namespace hft {

bool OrderBook::addOrder(Order order) {
    // Reject duplicate IDs
    if (orders_.contains(order.id)) {
        Logger::instance().warning("OrderBook",
            "Rejected duplicate OrderId " + std::to_string(order.id));
        return false;
    }

    // Reject invalid orders
    if (order.quantity <= 0 || order.remaining_qty <= 0) {
        Logger::instance().warning("OrderBook",
            "Rejected order with invalid quantity");
        return false;
    }
    if (order.type == OrderType::Limit && order.price <= 0.0) {
        Logger::instance().warning("OrderBook",
            "Rejected limit order with invalid price");
        return false;
    }

    OrderId id = order.id;
    Price price = order.price;
    Side side = order.side;

    // Store the order
    orders_.emplace(id, std::move(order));

    // Add to the appropriate price level
    if (side == Side::Buy) {
        bids_[price].push_back(id);
    } else {
        asks_[price].push_back(id);
    }

    return true;
}

bool OrderBook::cancelOrder(OrderId id) {
    auto it = orders_.find(id);
    if (it == orders_.end()) {
        return false;
    }

    Order& order = it->second;
    if (!order.is_active()) {
        return false;
    }

    // Remove from the price level
    removeFromLevel(order.side, order.price, id);

    // Mark as cancelled
    order.status = OrderStatus::Cancelled;

    return true;
}

bool OrderBook::modifyOrder(OrderId id, Quantity new_remaining_qty) {
    auto it = orders_.find(id);
    if (it == orders_.end()) {
        return false;
    }

    Order& order = it->second;
    if (!order.is_active()) {
        return false;
    }

    if (new_remaining_qty <= 0) {
        // Treat as cancel
        return cancelOrder(id);
    }

    Quantity old_remaining = order.remaining_qty;
    order.remaining_qty = new_remaining_qty;

    // If quantity increased, lose time priority (move to back)
    if (new_remaining_qty > old_remaining) {
        removeFromLevel(order.side, order.price, id);
        if (order.side == Side::Buy) {
            bids_[order.price].push_back(id);
        } else {
            asks_[order.price].push_back(id);
        }
    }

    return true;
}

std::optional<Price> OrderBook::bestBid() const {
    if (bids_.empty()) return std::nullopt;
    return bids_.begin()->first;
}

std::optional<Price> OrderBook::bestAsk() const {
    if (asks_.empty()) return std::nullopt;
    return asks_.begin()->first;
}

const Order* OrderBook::getOrder(OrderId id) const {
    auto it = orders_.find(id);
    if (it == orders_.end()) return nullptr;
    return &it->second;
}

bool OrderBook::contains(OrderId id) const {
    return orders_.contains(id);
}

Quantity OrderBook::quantityAtPrice(Side side, Price price) const {
    Quantity total = 0;

    if (side == Side::Buy) {
        auto it = bids_.find(price);
        if (it == bids_.end()) return 0;
        for (OrderId oid : it->second) {
            auto oit = orders_.find(oid);
            if (oit != orders_.end()) {
                total += oit->second.remaining_qty;
            }
        }
    } else {
        auto it = asks_.find(price);
        if (it == asks_.end()) return 0;
        for (OrderId oid : it->second) {
            auto oit = orders_.find(oid);
            if (oit != orders_.end()) {
                total += oit->second.remaining_qty;
            }
        }
    }
    return total;
}

OrderId OrderBook::frontOrderId(Side side, Price price) const {
    if (side == Side::Buy) {
        auto it = bids_.find(price);
        if (it == bids_.end() || it->second.empty()) return 0;
        return it->second.front();
    } else {
        auto it = asks_.find(price);
        if (it == asks_.end() || it->second.empty()) return 0;
        return it->second.front();
    }
}

Order* OrderBook::getMutableOrder(OrderId id) {
    auto it = orders_.find(id);
    if (it == orders_.end()) return nullptr;
    return &it->second;
}

void OrderBook::removeFilledOrder(OrderId id) {
    auto it = orders_.find(id);
    if (it == orders_.end()) return;

    Order& order = it->second;
    removeFromLevel(order.side, order.price, id);
}

bool OrderBook::removeFromLevel(Side side, Price price, OrderId id) {
    if (side == Side::Buy) {
        auto it = bids_.find(price);
        if (it == bids_.end()) return false;
        auto& dq = it->second;
        auto pos = std::find(dq.begin(), dq.end(), id);
        if (pos == dq.end()) return false;
        dq.erase(pos);
        cleanEmptyLevel(side, price);
        return true;
    } else {
        auto it = asks_.find(price);
        if (it == asks_.end()) return false;
        auto& dq = it->second;
        auto pos = std::find(dq.begin(), dq.end(), id);
        if (pos == dq.end()) return false;
        dq.erase(pos);
        cleanEmptyLevel(side, price);
        return true;
    }
}

void OrderBook::cleanEmptyLevel(Side side, Price price) {
    if (side == Side::Buy) {
        auto it = bids_.find(price);
        if (it != bids_.end() && it->second.empty()) {
            bids_.erase(it);
        }
    } else {
        auto it = asks_.find(price);
        if (it != asks_.end() && it->second.empty()) {
            asks_.erase(it);
        }
    }
}

} // namespace hft
