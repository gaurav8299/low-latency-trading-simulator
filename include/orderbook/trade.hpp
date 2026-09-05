#pragma once

#include "common/types.hpp"
#include <cstdint>

namespace hft {

struct Trade {
    uint64_t     trade_id;
    OrderId      buy_order_id;
    OrderId      sell_order_id;
    InstrumentId instrument;
    Price        price;         // execution price (resting order's price)
    Quantity     quantity;      // quantity traded
    Timestamp    timestamp;
};

} // namespace hft
