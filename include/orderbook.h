#pragma once

#include "order.h"
#include "trade.h"
#include <set>
#include <optional>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <shared_mutex>

class OrderBook {
public:
    OrderBook() = default;

    void addOrder(const Order& order);
    bool cancelOrder(int orderId);

    // Get best bid and best ask
    std::optional<Order> bestBid() const;
    std::optional<Order> bestAsk() const;

    const std::vector<Trade>& getTrades() const;
    std::vector<Trade> getRecentTrades(int n) const;

    std::optional<double> getSpread() const;
    int getVolumeAtPrice(double price, bool isBuy) const;
    void printDepth(int levels = 5) const;

    bool modifyOrder(int orderId, std::optional<double> newPrice, std::optional<int> newQuantity);

    // stats
    std::optional<double> getMidPrice() const;
    double getVWAP() const;
    double getOrderBookImbalance() const;
    int getTotalBidVolume() const;
    int getTotalAskVolume() const;
    int getVolumeInRange(double minPrice, double maxPrice, bool isBuy) const;

private:
    std::multiset<Order> bids;  // highest price first
    std::multiset<Order> asks;  // lowest price first
    std::vector<Trade> trades;
    std::vector<Order> stopOrders;
    mutable std::shared_mutex mtx;

    // Helper: find order by ID
    std::multiset<Order>::iterator findOrder(std::multiset<Order>& book, int orderId);
    std::unordered_map<int, std::multiset<Order>::iterator> orderIndex;

    // Helper for Fill-or-Kill validation
    bool canExecuteFillorKill(const Order& order) const;

    void checkStopOrders();
    double getLastTradePrice() const;
};
