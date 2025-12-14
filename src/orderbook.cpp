#include "orderbook.hpp"
#include <format>
#include <map>
#include <cmath>

void OrderBook::addOrder(const Order& order) {
    Order incomingOrder = order;

    if (incomingOrder.type == OrderType::FILL_OR_KILL) {
        if (!canExecuteFillorKill(incomingOrder)) {
            std::cout << std::format("FILL_OR_KILL order #{} cancelled - insufficient liquidity\n",
                                    incomingOrder.id);
            return; 
        }
    }

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

        std::cout << std::format("TRADE: {} shares at ${:.2f}\n", tradeQuantity, it->price);

        // create and store trade
        int buyOrderId = incomingOrder.isBuy ? incomingOrder.id : it->id;
        int sellOrderId = incomingOrder.isBuy ? it->id : incomingOrder.id;
        Trade trade(buyOrderId, sellOrderId, it->price, tradeQuantity);
        trades.push_back(trade);
        trade.print();

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

const std::vector<Trade>& OrderBook::getTrades() const {
    return trades;
}

std::vector<Trade> OrderBook::getRecentTrades(int n) const {
    std::vector<Trade> recent;

    // calculate how many trades to return
    size_t count = std::min(static_cast<size_t>(n), trades.size());
    size_t startIdx = trades.size() - count;

    // copy the last n trades
    for (size_t i = startIdx; i < trades.size(); ++i) {
        recent.push_back(trades[i]);
    }

    return recent;
}

void OrderBook::printTradeHistory() const {
    std::cout << "\n=== Trade History ===\n";
    std::cout << std::format("Total trades: {}\n\n", trades.size());

    if (trades.empty()) {
        std::cout << "No trades executed yet.\n";
        return;
    }

    for (const auto& trade : trades) {
        trade.print();
    }
    std::cout << "=====================\n";
}

std::optional<double> OrderBook::getSpread() const {
    auto bid = bestBid();
    auto ask = bestAsk();

    if (bid.has_value() && ask.has_value()) {
        return ask->price - bid->price;
    } else {
        return std::nullopt;
    }
}

int OrderBook::getVolumeAtPrice(double price, bool isBuy) const {
    const std::multiset<Order>& book = isBuy ? bids : asks;
    int totalVolume = 0;
    const double EPSILON = 0.0001;

    for (const auto& order: book) {
        if (std::abs(order.price - price) < EPSILON) {
            totalVolume += order.quantity;
        }
    }
    return totalVolume;
}

void OrderBook::printDepth(int levels) const {
    std::cout << "\n=== Order Book Depth ===\n";

    // Aggregate orders by price
    std::map<double, int> askLevels; // price -> total quantity
    std::map<double, int> bidLevels;

    for (const auto& order : asks) {
        askLevels[order.price] += order.quantity;
    }

    for (const auto& order : bids) {
        bidLevels[order.price] += order.quantity;
    }

    // Convert to vectors for easier indexing
    std::vector<std::pair<double, int>> askVector;
    std::vector<std::pair<double, int>> bidVector;

    // copy asks lowest to highest
    for (const auto& level : askLevels) {
        askVector.push_back(level);
    }

    // copy bids highest to lowest
    for (auto it = bidLevels.rbegin(); it != bidLevels.rend(); ++it) {
        bidVector.push_back(*it);
    }

    std::cout << "ASKS (Sellers):\n";

    // print asks in reverse (highest to lowest)
    int askCount = std::min(levels, static_cast<int>(askVector.size()));
    for (int i = askCount - 1; i >= 0; --i) {
        std::cout << std::format("  ${:>7.2f}  |  {:>4} shares\n",
                                askVector[i].first, askVector[i].second);
    }

    // print spread
    auto spread = getSpread();
    if (spread.has_value()) {
        std::cout << std::format("----------------------- SPREAD: ${:.2f} -----------------------\n",
                                spread.value());                        
    } else {
        std::cout << "----------------------- NO SPREAD -----------------------\n";
    }

    // print bids
    std::cout << "BIDS (Buyers):\n";
    int bidCount = std::min(levels, static_cast<int>(bidVector.size()));
    for (int i = 0; i < bidCount; ++i) {
        std::cout << std::format("  ${:>7.2f}  |  {:>4} shares\n",
                                bidVector[i].first, bidVector[i].second);
    }

    std::cout << "=========================\n";
}

bool OrderBook::modifyOrder(int orderId, std::optional<double> newPrice, std::optional<int> newQuantity) {
    // find order -> check bids first then asks
    auto it = findOrder(bids, orderId);
    bool isBid = (it != bids.end());

    if (it == bids.end()) {
        it = findOrder(asks, orderId);
        if (it == asks.end()) {
            return false; // order not found
        }
    }

    std::multiset<Order>& book = isBid ? bids : asks;
    Order modifiedOrder = *it;

    if (newPrice.has_value()) {
        modifiedOrder.price = newPrice.value();
    }

    if (newQuantity.has_value()) {
        modifiedOrder.quantity = newQuantity.value();
    }

    // update timestamp
    modifiedOrder.timestamp = std::chrono::system_clock::now();

    book.erase(it);
    book.insert(modifiedOrder);
    return true;
}

std::multiset<Order>::iterator OrderBook::findOrder(std::multiset<Order>& book, int orderId) {
    for (auto it = book.begin(); it != book.end(); ++it) {
        if (it->id == orderId) {
            return it;
        }
    }
    return book.end();
}

bool OrderBook::canExecuteFillorKill(const Order& order) const {
    const std::multiset<Order>& matchAgainst = order.isBuy ? asks : bids;
    int availableQuantity = 0;

    for (const auto& existingOrder : matchAgainst) {
        bool canMatch = order.isBuy ? 
            (order.price >= existingOrder.price) :
            (order.price <= existingOrder.price);

        if (!canMatch) {
            break;
        }

        availableQuantity += existingOrder.quantity;

        if (availableQuantity >= order.quantity) {
            return true; // sufficient quantity available
        }
    }
    return false; 
}