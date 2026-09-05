#pragma once

#include "common/types.hpp"
#include <chrono>

namespace hft {

struct Order {
    OrderId     id;
    InstrumentId instrument;
    Side        side;
    OrderType   type;
    Price       price;
    Quantity    quantity;        // original quantity (never changes)
    Quantity    remaining_qty;   // decreases as fills happen
    Timestamp   timestamp;
    OrderStatus status;

    // Factory: create a limit order
    static Order create_limit(OrderId id, const InstrumentId& instrument,
                              Side side, Price price, Quantity qty,
                              Timestamp ts) {
        return Order{
            .id            = id,
            .instrument    = instrument,
            .side          = side,
            .type          = OrderType::Limit,
            .price         = price,
            .quantity      = qty,
            .remaining_qty = qty,
            .timestamp     = ts,
            .status        = OrderStatus::New
        };
    }

    // Factory: create a market order (price = 0, matches at any price)
    static Order create_market(OrderId id, const InstrumentId& instrument,
                               Side side, Quantity qty, Timestamp ts) {
        return Order{
            .id            = id,
            .instrument    = instrument,
            .side          = side,
            .type          = OrderType::Market,
            .price         = 0.0,
            .quantity      = qty,
            .remaining_qty = qty,
            .timestamp     = ts,
            .status        = OrderStatus::New
        };
    }

    [[nodiscard]] bool is_filled() const {
        return remaining_qty <= 0;
    }

    [[nodiscard]] bool is_active() const {
        return status == OrderStatus::New ||
               status == OrderStatus::Accepted ||
               status == OrderStatus::PartiallyFilled;
    }

    void fill(Quantity qty) {
        remaining_qty -= qty;
        if (remaining_qty <= 0) {
            remaining_qty = 0;
            status = OrderStatus::Filled;
        } else {
            status = OrderStatus::PartiallyFilled;
        }
    }
};

} // namespace hft
