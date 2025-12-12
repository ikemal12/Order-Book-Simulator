#include "orderbook.hpp"

void OrderBook::addOrder(const Order& order) {
    Order incomingOrder = order;

    std::multiset<Order>& matchAgainst = incomingOrder.isBuy ? asks : bids;

    //try to match
    auto it = matchAgainst.begin();
    while (it != matchAgainst.end() && incomingOrder.quantity > 0) {
        bool canMatch = false;
        if (incomingOrder.isBuy) {
            canMatch = (incomingOrder.price >= it->price);
        } else {
            canMatch = (incomingOrder.price <= it->price);
        }

        if (!canMatch) {
            break;
        }

        int tradeQuantity = std::min(incomingOrder.quantity, it->quantity);

        std::cout << "TRADE: " << tradeQuantity << " shares at $" << it->price << "\n";

        incomingOrder.quantity -= tradeQuantity;

        Order modifiedOrder = *it;
        modifiedOrder.quantity -= tradeQuantity;

        it = matchAgainst.erase(it);

        if (modifiedOrder.quantity > 0) {
            matchAgainst.insert(modifiedOrder);
        }
    }

    // if theres remaining quantity, add to appropriate book
    if (incomingOrder.quantity > 0) {
        if (incomingOrder.isBuy) {
            bids.insert(incomingOrder);
        } else {
            asks.insert(incomingOrder);
        }
    }
}

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

void OrderBook::printTopOfBook() const {
    std::cout << "=== Top of Book ===\n";

    auto bid = bestBid();
    if (bid.has_value()) {
        std::cout << "Best Bid: ";
        bid.value().print();
    } else {
        std::cout << "Best Bid: None\n";
    }

    auto ask = bestAsk();
    if (ask.has_value()) {
        std::cout << "Best Ask: ";
        ask.value().print();
    } else {
        std::cout << "Best Ask: None\n";
    }

    std::cout << "===================\n";   
}

std::multiset<Order>::iterator OrderBook::findOrder(std::multiset<Order>& book, int orderId) {
    for (auto it = book.begin(); it != book.end(); ++it) {
        if (it->id == orderId) {
            return it;
        }
    }
    return book.end();
}