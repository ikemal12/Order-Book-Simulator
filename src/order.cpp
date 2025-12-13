#include "order.h"

void Order::print() const {
        std::cout << std::format("Order ID: {}, Price: {:.2f}, Quantity: {}, Side: {}, Type: {}\n",
                  id, price, quantity, isBuy ? "Buy" : "Sell",
                  type == OrderType::LIMIT ? "Limit" : "Market");
}