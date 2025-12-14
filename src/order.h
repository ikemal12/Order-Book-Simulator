#ifndef ORDER_BOOK_H
#define ORDER_BOOK_H

#include <string>
#include <chrono>
#include <format>
#include <iostream>

enum class OrderType {
    MARKET,
    LIMIT,
    FILL_OR_KILL,
};

class Order {
public:
    int id;
    double price;
    int quantity;
    bool isBuy;
    std::chrono::time_point<std::chrono::system_clock> timestamp;
    OrderType type;

    Order(int id, double price, int quantity, bool isBuy, OrderType type = OrderType::LIMIT)
        : id(id), price(price), quantity(quantity), isBuy(isBuy), type(type), 
        timestamp(std::chrono::system_clock::now()) {}

    void print() const;

    bool operator<(const Order& other) const {
        if (isBuy) {
            return price > other.price || 
                (price == other.price && timestamp < other.timestamp);
        } else {
            return price < other.price || 
                (price == other.price && timestamp < other.timestamp);
        }
    }
};

#endif 