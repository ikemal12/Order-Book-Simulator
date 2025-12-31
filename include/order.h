#pragma once
#include <chrono>

enum class OrderType {
    MARKET,
    LIMIT,
    FILL_OR_KILL,
    IMMEDIATE_OR_CANCEL,
    STOP_LOSS
};

class Order {
public:
    int id;
    double price;
    int quantity;
    bool isBuy;
    std::chrono::time_point<std::chrono::system_clock> timestamp;
    OrderType type;
    double stopPrice; // trigger price for stop orders

    explicit Order(int id, double price, int quantity, bool isBuy, OrderType type = OrderType::LIMIT, double stopPrice = 0.0)
        : id(id), price(price), quantity(quantity), isBuy(isBuy), type(type), 
        timestamp(std::chrono::system_clock::now()),
        stopPrice(stopPrice) {}

    bool operator<(const Order& other) const noexcept {
        if (isBuy) {
            return price > other.price || 
                (price == other.price && timestamp < other.timestamp);
        } else {
            return price < other.price || 
                (price == other.price && timestamp < other.timestamp);
        }
    }
};
