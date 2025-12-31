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
    [[nodiscard]] std::optional<Order> bestBid() const noexcept;
    [[nodiscard]] std::optional<Order> bestAsk() const noexcept;
    [[nodiscard]] std::vector<Trade> getRecentTrades(int n) const noexcept;
    [[nodiscard]] std::optional<double> getSpread() const noexcept;
    int getVolumeAtPrice(double price, bool isBuy) const noexcept;
    void printDepth(int levels = 5) const;
    [[nodiscard]] std::optional<double> getMidPrice() const noexcept;
    [[nodiscard]] double getVWAP() const noexcept;
    [[nodiscard]] VolumeInfo getVolumeInfo() const noexcept;

private:
    std::multiset<Order> bids;  // highest price first
    std::multiset<Order> asks;  // lowest price first
    std::vector<Trade> trades;
    std::vector<Order> stopOrders;
    std::unordered_map<int, std::multiset<Order>::iterator> orderIndex;
    bool canExecuteFillorKill(const Order& order) const noexcept;
    void checkStopOrders();
    double getLastTradePrice() const noexcept;
};
