# Order Book Simulator

Currently supports:

* price-time priority matching
* efficient O(1) order lookup, cancellation, and modification
* multiple order types including Market, Limit, Stop Loss, Fill-or-Kill and Immediate-or-Cancel
* core market data and statistics including best bid/ask, VWAP, spread, depth, and imbalance

## Performance

The order book is currently running single-threaded and O(1) for all read/write operations. With a benchmark setup of 20% writes and 80% reads, I was able to achieve an average throughput of ~1.14M operations/second:

```
Operations:    1000 | Time: 0.0008s | Throughput:    1295001 operations/sec
Operations:   10000 | Time: 0.0098s | Throughput:    1019981 operations/sec
Operations:  100000 | Time: 0.0921s | Throughput:    1085897 operations/sec
Operations: 1000000 | Time: 0.8604s | Throughput:    1162280 operations/sec
```

