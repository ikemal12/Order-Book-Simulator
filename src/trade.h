#ifndef TRADE_H
#define TRADE_H

#include <chrono>
#include <format>
#include <iostream>

class Trade {
public: 
    int buyOrderId;
    int sellOrderId;
    double price;
    int quantity;
    std::chrono::time_point<std::chrono::system_clock> timestamp;

    Trade(int buyOrderId, int sellOrderId, double price, int quantity)
        : buyOrderId(buyOrderId), sellOrderId(sellOrderId), price(price), quantity(quantity),
          timestamp(std::chrono::system_clock::now()) {}

    void print() const {
        auto seconds = std::chrono::duration_cast<std::chrono::seconds>(timestamp.time_since_epoch()).count();
        std::cout << std::format("TRADE: Buy Order #{} matched Sell Order #{} | {} shares @ ${:.2f} | Timestamp: {}\n",
                                buyOrderId, sellOrderId, quantity, price, seconds);
    }
};

#endif