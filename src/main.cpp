#include "orderbook.hpp"
#include <iostream>
#include <format>

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
    std::cout << std::format("Cancel {}\n", cancelled ? "successful" : "failed");

    book.printTopOfBook();
    std::cout << "\n";

    // Test 5: Print trade history
    book.printTradeHistory();

    // Test 6: Get recent trades
    std::cout << "\nGetting last 2 trades:\n";
    auto recentTrades = book.getRecentTrades(2);
    for (const auto& trade : recentTrades) {
        trade.print();
    }

    // Test 7: Modify order price
    std::cout << "\nModifying order #5 price to $99.75...\n";
    book.modifyOrder(5, 99.75, std::nullopt);
    book.printDepth(5);

    // Test 8: Modify order quantity
    std::cout << "\nModifying order #3 quantity to 20...\n";
    book.modifyOrder(3, std::nullopt, 20);
    book.printDepth(5);

    // Test 9: Modify both price and quantity
    std::cout << "\nModifying order #2 to $99.90 and 10 shares...\n";
    book.modifyOrder(2, 99.90, 10);
    book.printDepth(5);

    // Testing fill or kill
    std::cout << "\n=== Testing Fill-or-Kill Orders ===\n";
    book.printDepth(5);

    std::cout << "\nTest 1: FOK buy 100 shares @ $102 (should fail)...\n";
    book.addOrder(Order(200, 102.00, 100, true, OrderType::FILL_OR_KILL));
    book.printDepth(5);

    std::cout << "\nTest 2: FOK buy 5 shares @ $102 (should succeed)...\n";
    book.addOrder(Order(201, 102.00, 5, true, OrderType::FILL_OR_KILL));
    book.printDepth(5);

    std::cout << "\nTest 3: FOK sell 10 shares @98 (check if it works)...\n";
    book.addOrder(Order(202, 98.00, 10, false, OrderType::FILL_OR_KILL));
    book.printDepth(5);

    // test IOC
    std::cout << "\n=== Testing Immediate-or-Cancel Orders ===\n";
    book.printDepth(5);

    // test 1: IOC that partially fills
    std::cout << "\nTest 1: IOC buy 50 shares @ $102 (should partially fill)...\n";
    book.addOrder(Order(300, 102.00, 50, true, OrderType::IMMEDIATE_OR_CANCEL));
    book.printDepth(5);

    // test 2: IOC that fully fills
    std::cout << "\nTest 2: IOC buy 3 shares @ $102 (should fill completely)...\n";
    book.addOrder(Order(301, 102.00, 3, true, OrderType::IMMEDIATE_OR_CANCEL));
    book.printDepth(5);

    // test 3: IOC with no matches
    std::cout << "\nTest 3: IOC buy 10 shares @ $95 (no match, all cancelled)...\n";
    book.addOrder(Order(302, 95.00, 10, true, OrderType::IMMEDIATE_OR_CANCEL));
    book.printDepth(5);
    
    return 0;
}