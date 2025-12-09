#ifndef ORDER_BOOK_HPP
#define ORDER_BOOK_HPP

#include <set>
#include <optional>
#include <iostream>
#include "order.h"

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

private:
    std::multiset<Order> bids;  // highest price first
    std::multiset<Order> asks;  // lowest price first

    // Helper: find order by ID
    std::multiset<Order>::iterator findOrder(std::multiset<Order>& book, int orderId);
};

#endif 