#pragma once
#include "order.h"
#include "trade.h"
#include <set>
#include <optional>
#include <vector>
#include <unordered_map>

struct VolumeInfo {
    int bidVolume;
    int askVolume;
    double imbalance;
};

class OrderBook {
public:
    OrderBook() = default;
    void addOrder(const Order& order);
    bool cancelOrder(int orderId);
    bool modifyOrder(int orderId, std::optional<double> newPrice, std::optional<int> newQuantity);
    std::optional<Order> bestBid() const;
    std::optional<Order> bestAsk() const;
    std::vector<Trade> getRecentTrades(int n) const;
    std::optional<double> getSpread() const;
    int getVolumeAtPrice(double price, bool isBuy) const;
    void printDepth(int levels = 5) const;
    std::optional<double> getMidPrice() const;
    double getVWAP() const;
    VolumeInfo getVolumeInfo() const;

private:
    std::multiset<Order> bids;  // highest price first
    std::multiset<Order> asks;  // lowest price first
    std::vector<Trade> trades;
    std::vector<Order> stopOrders;
    std::unordered_map<int, std::multiset<Order>::iterator> orderIndex;
    bool canExecuteFillorKill(const Order& order) const;
    void checkStopOrders();
    double getLastTradePrice() const;
};
