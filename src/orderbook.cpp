#include "orderbook.h"
#include <iostream>
#include <format>
#include <map>
#include <cmath>

void OrderBook::addOrder(const Order& order) {
    Order incomingOrder = order;
    if (incomingOrder.type == OrderType::STOP_LOSS) {
        stopOrders.push_back(incomingOrder);
        return;
    }
    if (incomingOrder.type == OrderType::FILL_OR_KILL) {
        if (!canExecuteFillorKill(incomingOrder)) {
            return; 
        }
    }

    std::multiset<Order>& matchAgainst = incomingOrder.isBuy ? asks : bids;
    auto it = matchAgainst.begin();
    while (it != matchAgainst.end() && incomingOrder.quantity > 0) {
        bool canMatch = false;
        if (incomingOrder.type == OrderType::MARKET) { // Market orders always match at any price
            canMatch = true;
        }
        else if (incomingOrder.isBuy) { // Limit orders check price compatibility
            canMatch = (incomingOrder.price >= it->price);
        } else {
            canMatch = (incomingOrder.price <= it->price);
        }

        if (!canMatch) {
            break;
        }
        int tradeQuantity = std::min(incomingOrder.quantity, it->quantity);
        int buyOrderId = incomingOrder.isBuy ? incomingOrder.id : it->id;
        int sellOrderId = incomingOrder.isBuy ? it->id : incomingOrder.id;
        Trade trade(buyOrderId, sellOrderId, it->price, tradeQuantity);
        trades.push_back(trade);

        incomingOrder.quantity -= tradeQuantity;
        Order modifiedOrder = *it;
        modifiedOrder.quantity -= tradeQuantity;    
        orderIndex.erase(it->id);
        it = matchAgainst.erase(it);

        if (modifiedOrder.quantity > 0) {
            auto newIt = matchAgainst.insert(modifiedOrder);
            orderIndex[modifiedOrder.id] = newIt;
        }
    }

    if (incomingOrder.quantity > 0) {
        // IOC: dont add to book -> just cancel remainder
        if (incomingOrder.type == OrderType::IMMEDIATE_OR_CANCEL) {
            return;
        }
        auto it = incomingOrder.isBuy ? bids.insert(incomingOrder) : asks.insert(incomingOrder);
        orderIndex[incomingOrder.id] = it;
    }
    checkStopOrders();
}

bool OrderBook::cancelOrder(int orderId) {
    auto indexIt = orderIndex.find(orderId);
    if (indexIt == orderIndex.end()) {
        return false;
    }
    (indexIt->second->isBuy ? bids : asks).erase(indexIt->second);
    orderIndex.erase(indexIt);
    return true; 
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

std::vector<Trade> OrderBook::getRecentTrades(int n) const {
    size_t count = std::min(static_cast<size_t>(n), trades.size());
    return std::vector<Trade>(trades.end() - count, trades.end());
}

std::optional<double> OrderBook::getSpread() const {
    if (bids.empty() || asks.empty()) {
        return std::nullopt;
    }
    return asks.begin()->price - bids.begin()->price;
}

void OrderBook::printDepth(int levels) const {
    std::cout << "\n=== Order Book Depth ===\n";
    std::map<double, int> askLevels, bidLevels; // price -> total quantity
    for (const auto& order : asks) askLevels[order.price] += order.quantity;
    for (const auto& order : bids) bidLevels[order.price] += order.quantity;
    std::vector<std::pair<double, int>> askVector(askLevels.begin(), askLevels.end());
    std::vector<std::pair<double, int>> bidVector(bidLevels.rbegin(), bidLevels.rend());

    std::cout << "ASKS (Sellers):\n";
    int askCount = std::min(levels, static_cast<int>(askVector.size()));
    for (int i = askCount - 1; i >= 0; --i) { 
        std::cout << std::format("  ${:>7.2f}  |  {:>4} shares\n", askVector[i].first, askVector[i].second);
    }
    auto spread = getSpread();
    if (spread.has_value()) {
    std::cout << std::format("----------------------- SPREAD: ${:.2f} -----------------------\n", spread.value());
    } else {
        std::cout << "----------------------- NO SPREAD -----------------------\n";
    }
    std::cout << "BIDS (Buyers):\n";
    int bidCount = std::min(levels, static_cast<int>(bidVector.size()));
    for (int i = 0; i < bidCount; ++i) {
        std::cout << std::format("  ${:>7.2f}  |  {:>4} shares\n", bidVector[i].first, bidVector[i].second);
    }
    std::cout << "=========================\n";
}

bool OrderBook::modifyOrder(int orderId, std::optional<double> newPrice, std::optional<int> newQuantity) {
    auto indexIt = orderIndex.find(orderId);
    if (indexIt == orderIndex.end()) {
        return false;
    }

    auto it = indexIt->second;
    std::multiset<Order>& book = it->isBuy ? bids : asks;
    Order modifiedOrder = *it;
    if (newPrice.has_value()) {
        modifiedOrder.price = newPrice.value();
    }

    if (newQuantity.has_value()) {
        modifiedOrder.quantity = newQuantity.value();
    }

    modifiedOrder.timestamp = std::chrono::system_clock::now(); // update timestamp
    orderIndex.erase(indexIt);
    book.erase(it);
    auto newIt = book.insert(modifiedOrder);
    orderIndex[modifiedOrder.id] = newIt;
    return true;
}

std::optional<double> OrderBook::getMidPrice() const {
    if (bids.empty() || asks.empty()) {
        return std::nullopt;
    }
    return (bids.begin()->price + asks.begin()->price) / 2.0;
}

VolumeInfo OrderBook::getVolumeInfo() const {
    int bidVol = 0, askVol = 0;
    for (const auto& order : bids) bidVol += order.quantity;
    for (const auto& order : asks) askVol += order.quantity;
    double imbalance = (askVol == 0) ? (bidVol > 0 ? 999.0 : 1.0) : static_cast<double>(bidVol) / askVol;
    return {bidVol, askVol, imbalance};
}

double OrderBook::getVWAP() const {
    if (trades.empty()) {
        return 0.0;
    }
    double totalValue = 0.0; // sum of (price * quantity)
    int totalVolume = 0;
    for (const auto& trade : trades) {
        totalValue += trade.price * trade.quantity;
        totalVolume += trade.quantity;
    }

    if (totalVolume == 0) {
        return 0.0;
    }
    return totalValue / totalVolume;
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
            return true; 
        }
    }
    return false; 
}

double OrderBook::getLastTradePrice() const {
    if (trades.empty()) {
        return 0.0; 
    }
    return trades.back().price;
}

void OrderBook::checkStopOrders() {
    if (trades.empty()) {
        return;
    }
    double lastPrice = getLastTradePrice();
    for (auto it = stopOrders.begin(); it != stopOrders.end(); ) {
        bool shouldTrigger = false;
        if (it->isBuy) {
            shouldTrigger = (lastPrice >= it->stopPrice);
        } else {
            shouldTrigger = (lastPrice <= it->stopPrice);
        }

        if (shouldTrigger) {
            Order marketOrder = *it;
            marketOrder.type = OrderType::MARKET;
            it = stopOrders.erase(it);
            addOrder(marketOrder);
        } else {
            ++it;
        }
    }
}