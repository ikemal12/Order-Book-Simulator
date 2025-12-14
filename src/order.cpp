#include "order.h"

void Order::print() const {
    std::string typeStr;
    switch(type) {
        case OrderType::MARKET:
            typeStr = "Market";
            break;
        case OrderType::LIMIT:
            typeStr = "Limit";
            break;
        case OrderType::FILL_OR_KILL:
            typeStr = "Fill-or-Kill";
            break;
        case OrderType::IMMEDIATE_OR_CANCEL:
            typeStr = "Immediate-or-Cancel";
            break;
        case OrderType::STOP_LOSS:
            typeStr = std::format("Stop Loss @ ${:.2f}", stopPrice);
            break;
    }

    std::cout << std::format("Order ID: {}, Price: {:.2f}, Quantity: {}, Side: {}, Type: {}\n",
                id, price, quantity, isBuy ? "Buy" : "Sell", typeStr);
}