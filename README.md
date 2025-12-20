# Limit Order Book Simulator

## Roadmap

* Basic Order class (done)
* OrderBook with matching (done)
* Trade history (done)
* Levels, full book display (done)
* Order modification (done)
* Different types eg. Stop Loss, FOK, Immediate-or-Cancel (done)
* Market data/stats eg mid, VWAP, ratio, volume (doing)
* Performance optimisation eg multithreading, memory pool allocation
* Testing/benchmarking
* ...

Maybes
* Persistence/serialization
* CLI simulator
* real-time websockets API

Things to check
* Are we matching orders by type or by timestamp?
* Vector vs unordered map for storing and accessing orders
* for market orders setting quantity to 0
