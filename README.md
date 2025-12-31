# Order Book Simulator

Currently supports:

* price-time priority matching
* efficient O(1) order lookup, cancellation, and modification
* multiple order types including Market, Limit, Stop Loss, Fill-or-Kill and Immediate-or-Cancel
* core market data and statistics including best bid/ask, VWAP, spread, depth, and imbalance

## Performance

The order book is currently running single-threaded and O(1) for all read/write operations. With a benchmark setup of 20% writes and 80% reads, I was able to achieve an average throughput of ~15.67M operations/second:

```
Operations:    1000 | Time: 0.0001s | Throughput:   18094089 operations/sec
Operations:   10000 | Time: 0.0006s | Throughput:   17891221 operations/sec
Operations:  100000 | Time: 0.0066s | Throughput:   15155036 operations/sec
Operations: 1000000 | Time: 0.0867s | Throughput:   11533985 operations/sec
```

