#include "orderbook.hpp"
#include <iostream>

int main() {
    OrderBook book;

    std::cout << "=== Limit Order Book Simulator ===\n\n";

    // Test 1: Add some sell orders (asks)
    std::cout << "Adding sell orders...\n";
    book.addOrder(Order(1, 100.50, 10, false)); 
    book.addOrder(Order(2, 100.00, 5, false));
    book.addOrder(Order(3, 101.00, 15, false));

    book.printTopOfBook();
    std::cout << "\n";

    // Test 2: Add some buy orders (bids)
    std::cout << "Adding buy orders...\n";
    book.addOrder(Order(4, 99.50, 8, true));
    book.addOrder(Order(5, 99.00, 12, true));

    book.printTopOfBook();
    std::cout << "\n";

    // Test 3: Add a matching buy order
    std::cout << "Adding buy order that matches...\n";
    book.addOrder(Order(6, 100.50, 7, true));

    book.printTopOfBook();
    std::cout << "\n";

    // Test 4: Cancel an order
    std::cout << "Cancelling order #4...\n";
    bool cancelled = book.cancelOrder(4);
    std::cout << "Cancel " << (cancelled ? "successful" : "failed") << "\n";

    book.printTopOfBook();
    std::cout << "\n";

    return 0;
}