# Order Book Simulator

A high-performance C++20 order book simulator with price-time matching, multiple order types, and thread safety.

## Features

- **Order Matching:** Price-time matching algorithm with O(1) order lookup (hash map indexing) and order cancellation/modification
- **Order Types:** Limit, Market, Fill-or-Kill (FOK), Immediate-or-Cancel (IOC), Stop Loss 
- **Thread Safety:** `std::shared_mutex` reader-writer locks for concurrent access
- **Market Data & Statistics:** Best bid/ask, Mid-price, Volume-weighted average price (VWAP), imbalance, spread, depth 

## Architecture

### Data Structures

- **Order Storage**: `std::multiset<Order>` for automatic price-time sorting
- **Order Index**: `std::unordered_map<int, iterator>` for O(1) order lookups
- **Trade History**: `std::vector<Trade>` for chronological trade records
- **Stop Orders**: Separate queue for pending stop loss orders

### Matching Algorithm

Orders are matched using price-time priority:
1. Best price gets priority (highest bid, lowest ask)
2. Orders at the same price level are matched by timestamp (FIFO)
3. Market orders bypass price checks and match at any price
4. Partial fills are supported - orders can be split across multiple trades

### Concurrency Model

Thread safety is implemented using a single `std::shared_mutex`:
- **Write operations** (add, cancel, modify) acquire exclusive locks (`std::unique_lock`)
- **Read operations** (queries, statistics) acquire shared locks (`std::shared_lock`)
- Multiple readers can access simultaneously, but writers have exclusive access

**Note**: Current implementation prioritizes correctness over throughput. Lock contention limits scalability under high concurrent load. Will need to optimize.

## Building

### Requirements

- GCC 15+ (or any C++20 compatible compiler)
- GNU Make (or mingw32-make on Windows)
- C++20 standard library with `<format>` support

### Compilation

```bash
# Build main simulator
make

# Build benchmark
make benchmark

# Clean build artifacts
make clean
```

On Windows with MinGW, use `mingw32-make` instead of `make`.

### Compiler Flags

- `-std=c++20` - Enable C++20 features
- `-Iinclude` - Include directory for headers
- `-lstdc++exp` - Experimental C++ library features (for `std::format`)

## Usage

### Running the Simulator

```bash
# Run main demo
./orderbook

# Run performance benchmark
./benchmark
```

### Example

```cpp
#include "orderbook.h"

int main() {
    OrderBook book;
    
    // Add orders
    Order buyOrder(1, 100.0, 10, true, OrderType::LIMIT);
    Order sellOrder(2, 100.5, 5, false, OrderType::LIMIT);
    
    book.addOrder(buyOrder);
    book.addOrder(sellOrder);
    
    // Query market data
    auto bestBid = book.getBestBid();
    auto spread = book.getSpread();
    double vwap = book.getVWAP(100);
    
    // Display order book
    book.printDepth(5);
    
    return 0;
}
```

