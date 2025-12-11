#include "orderbook.hpp"

std::optional<Order> OrderBook::bestBid() const {
    if (bids.empty()) {
        return std::nullopt;
    }
    return *bids.begin();
}

std::optional<Order> OrderBook::bestAsk() const {
    if (asks.empty()) {
        return std::nullopt;
    }
    return *asks.begin();
}

std::multiset<Order>::iterator OrderBook::findOrder(std::multiset<Order>& book, int orderId) {
    for (auto it = book.begin(); it != book.end(); ++it) {
        if (it->id == orderId) {
            return it;
        }
    }
    return book.end();
}