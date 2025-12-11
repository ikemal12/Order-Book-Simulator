#include "orderbook.hpp"

std::multiset<Order>::iterator OrderBook::findOrder(std::multiset<Order>& book, int orderId) {
    for (auto it = book.begin(); it != book.end(); ++it) {
        if (it->id == orderId) {
            return it;
        }
    }
    return book.end();
}