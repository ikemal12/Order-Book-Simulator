#pragma once

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
          
};
