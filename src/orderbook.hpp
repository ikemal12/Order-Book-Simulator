#ifndef ORDER_BOOK_HPP
#define ORDER_BOOK_HPP

#include <set>
#include <optional>
#include <iostream>
#include "order.h"
#include "trade.h"
#include <vector>

class OrderBook {
public:
    OrderBook() = default;

    // Add new order to book
    void addOrder(const Order& order);

    // Remove an order by ID
    bool cancelOrder(int orderId);

    // Get best bid and best ask
    std::optional<Order> bestBid() const;
    std::optional<Order> bestAsk() const;

    // Print top of the book
    void printTopOfBook() const;

    const std::vector<Trade>& getTrades() const;
    std::vector<Trade> getRecentTrades(int n) const;
    void printTradeHistory() const;

    std::optional<double> getSpread() const;
    int getVolumeAtPrice(double price, bool isBuy) const;
    void printDepth(int levels = 5) const;

    bool modifyOrder(int orderId, std::optional<double> newPrice, std::optional<int> newQuantity);

private:
    std::multiset<Order> bids;  // highest price first
    std::multiset<Order> asks;  // lowest price first
    std::vector<Trade> trades;

    // Helper: find order by ID
    std::multiset<Order>::iterator findOrder(std::multiset<Order>& book, int orderId);
};

#endif 