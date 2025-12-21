#include "orderbook.h"
#include <format>
#include <map>
#include <cmath>
#include <mutex>

void OrderBook::addOrder(const Order& order) {
    std::unique_lock<std::shared_mutex> lock(mtx);
    Order incomingOrder = order;

    if (incomingOrder.type == OrderType::STOP_LOSS) {
        stopOrders.push_back(incomingOrder);
        //std::cout << std::format("Stop Loss order #{} added to queue (trigger @ ${:.2f})\n",
                                //incomingOrder.id, incomingOrder.stopPrice);
        return;
    }

    if (incomingOrder.type == OrderType::FILL_OR_KILL) {
        if (!canExecuteFillorKill(incomingOrder)) {
            //std::cout << std::format("FILL_OR_KILL order #{} cancelled - insufficient liquidity\n",
                                    //incomingOrder.id);
            return; 
        }
    }

    std::multiset<Order>& matchAgainst = incomingOrder.isBuy ? asks : bids;

    //try to match
    auto it = matchAgainst.begin();
    while (it != matchAgainst.end() && incomingOrder.quantity > 0) {
        bool canMatch = false;
        
        // Market orders always match at any price
        if (incomingOrder.type == OrderType::MARKET) {
            canMatch = true;
        }
        // Limit orders check price compatibility
        else if (incomingOrder.isBuy) {
            canMatch = (incomingOrder.price >= it->price);
        } else {
            canMatch = (incomingOrder.price <= it->price);
        }

        if (!canMatch) {
            break;
        }

        int tradeQuantity = std::min(incomingOrder.quantity, it->quantity);

        //std::cout << std::format("TRADE: {} shares at ${:.2f}\n", tradeQuantity, it->price);

        // create and store trade
        int buyOrderId = incomingOrder.isBuy ? incomingOrder.id : it->id;
        int sellOrderId = incomingOrder.isBuy ? it->id : incomingOrder.id;
        Trade trade(buyOrderId, sellOrderId, it->price, tradeQuantity);
        trades.push_back(trade);

        incomingOrder.quantity -= tradeQuantity;

        Order modifiedOrder = *it;
        modifiedOrder.quantity -= tradeQuantity;

        // remove from index before erasing
        orderIndex.erase(it->id);
        it = matchAgainst.erase(it);

        if (modifiedOrder.quantity > 0) {
            auto newIt = matchAgainst.insert(modifiedOrder);
            orderIndex[modifiedOrder.id] = newIt;
        }
    }

    // if theres remaining quantity, add to appropriate book
    if (incomingOrder.quantity > 0) {
        // IOC: dont add to book -> just cancel remainder
        if (incomingOrder.type == OrderType::IMMEDIATE_OR_CANCEL) {
            //std::cout << std::format("IOC order #{}: {} shares cancelled (unfilled)\n",
                                    //incomingOrder.id, incomingOrder.quantity);
            return;
        }

        // regular orders -> add to book and index
        auto it = incomingOrder.isBuy ? bids.insert(incomingOrder) : asks.insert(incomingOrder);
        orderIndex[incomingOrder.id] = it;
    }
    checkStopOrders();
}

bool OrderBook::cancelOrder(int orderId) {
    std::unique_lock<std::shared_mutex> lock(mtx);
    auto indexIt = orderIndex.find(orderId);

    if (indexIt == orderIndex.end()) {
        return false;
    }

    auto orderIt = indexIt->second;

    if (orderIt->isBuy) {
        bids.erase(orderIt);
    } else {
        asks.erase(orderIt);
    }
    
    orderIndex.erase(indexIt);

    return true; 
}

std::optional<Order> OrderBook::bestBid() const {
    std::shared_lock<std::shared_mutex> lock(mtx);

    if (bids.empty()) {
        return std::nullopt;
    }
    return *bids.begin();
}

std::optional<Order> OrderBook::bestAsk() const {
    std::shared_lock<std::shared_mutex> lock(mtx);

    if (asks.empty()) {
        return std::nullopt;
    }
    return *asks.begin();
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

std::optional<double> OrderBook::getSpread() const {
    std::shared_lock<std::shared_mutex> lock(mtx);

    if (bids.empty() || asks.empty()) {
        return std::nullopt;
    }

    return asks.begin()->price - bids.begin()->price;
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
    std::shared_lock<std::shared_mutex> lock(mtx);
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
    std::unique_lock<std::shared_mutex> lock(mtx);
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

    // update timestamp
    modifiedOrder.timestamp = std::chrono::system_clock::now();

    orderIndex.erase(indexIt);
    book.erase(it);

    auto newIt = book.insert(modifiedOrder);
    orderIndex[modifiedOrder.id] = newIt;
   
    return true;
}

std::optional<double> OrderBook::getMidPrice() const {
    std::shared_lock<std::shared_mutex> lock(mtx);

    if (bids.empty() || asks.empty()) {
        return std::nullopt;
    }

    return (bids.begin()->price + asks.begin()->price) / 2.0;
}

int OrderBook::getTotalBidVolume() const {
    int total = 0;
    for (const auto& order : bids) {
        total += order.quantity;
    }
    return total;
}

int OrderBook::getTotalAskVolume() const {
    int total = 0;
    for (const auto& order : asks) {
        total += order.quantity;
    }
    return total;
}

double OrderBook::getOrderBookImbalance() const {
    std::shared_lock<std::shared_mutex> lock(mtx);

    int bidVol = getTotalBidVolume();
    int askVol = getTotalAskVolume();

    if (askVol == 0) {
        return bidVol > 0 ? 999.0 : 1.0; // avoid division by zero
    }

    return static_cast<double>(bidVol) / static_cast<double>(askVol);
}

double OrderBook::getVWAP() const {
    std::shared_lock<std::shared_mutex> lock(mtx);

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

std::multiset<Order>::iterator OrderBook::findOrder(std::multiset<Order>& book, int orderId) {
    auto indexIt = orderIndex.find(orderId);

    if (indexIt == orderIndex.end()) {
        return book.end();
    }

    return indexIt->second; 
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

double OrderBook:: getLastTradePrice() const {
    if (trades.empty()) {
        return 0.0; // no trades yet
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
            //std::cout << std::format("STOP ORDER TRIGGERED: Order #{} at ${:.2f}\n",
                                    //it->id, it->stopPrice);
            
            Order marketOrder = *it;
            marketOrder.type = OrderType::MARKET;
            
            it = stopOrders.erase(it);

            addOrder(marketOrder);
        } else {
            ++it;
        }
    }
}