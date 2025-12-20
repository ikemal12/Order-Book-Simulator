# Limit Order Book Simulator

## Roadmap

* Basic Order class (done)
* OrderBook with matching (done)
* Trade history (done)
* Levels, full book display (done)
* Order modification (done)
* Different types eg. Stop Loss, FOK, Immediate-or-Cancel (done)
* Market data/stats eg mid, VWAP, ratio, volume (done)
* Performance optimisations eg thread safety, memory pooling (doing)
* Testing/benchmarking
* ...

Maybes
* Persistence/serialization
* CLI simulator
* real-time websockets API

Things to check
* Are we matching orders by type or by timestamp?
    * Currently matching by price-time priority (timestamp secondary, ie. order type does not affect matching order)
* Vector vs unordered map for storing and accessing orders?
    * currently using multiset for storing and unordered_map for lookup
* for market orders setting quantity to 0
    * no, but need to fix matching logic
