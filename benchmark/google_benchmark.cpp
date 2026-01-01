#include <benchmark/benchmark.h>
#include <orderbook.h>
#include <random>

static void BM_AddOrder(benchmark::State& state) {
    OrderBook book;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> price(99.0, 101.0);
    
    int id = 0;
    for (auto _ : state) {
        Order order(id++, price(gen), 100, true, OrderType::LIMIT);
        book.addOrder(order);
    }
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_AddOrder);

static void BM_BestBid(benchmark::State& state) {
    OrderBook book;
    for (int i = 0; i < 1000; ++i) {
        book.addOrder(Order(i, 100.0 + i*0.01, 10, true, OrderType::LIMIT));
    }
    
    for (auto _ : state) {
        benchmark::DoNotOptimize(book.bestBid());
    }
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_BestBid);

BENCHMARK_MAIN();