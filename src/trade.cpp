#include "trade.h"

void Trade::print() const {
        auto seconds = std::chrono::duration_cast<std::chrono::seconds>(timestamp.time_since_epoch()).count();
        std::cout << std::format("TRADE: Buy Order #{} matched Sell Order #{} | {} shares @ ${:.2f} | Timestamp: {}\n",
                                buyOrderId, sellOrderId, quantity, price, seconds);
};