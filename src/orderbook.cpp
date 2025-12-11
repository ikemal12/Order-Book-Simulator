#include "orderbook.hpp"

bool OrderBook::cancelOrder(int orderId) {
    // try to find in bids first
    auto it = findOrder(bids, orderId);
    if (it != bids.end()) {
        bids.erase(it);
        return true;
    }

    // not in bids, try asks
    it = findOrder(asks, orderId);
    if (it != asks.end()) {
        asks.erase(it);
        return true;
    }

    return false; // not found anywhere
}

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