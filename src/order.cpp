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
    }

    std::cout << std::format("Order ID: {}, Price: {:.2f}, Quantity: {}, Side: {}, Type: {}\n",
                id, price, quantity, isBuy ? "Buy" : "Sell", typeStr);
}