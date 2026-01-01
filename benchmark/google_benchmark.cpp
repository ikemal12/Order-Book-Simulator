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

static void BM_CancelOrder(benchmark::State& state) {
    int nextId = 0;
    for (auto _ : state) {
        state.PauseTiming();  
        OrderBook book;
        for (int i = 0; i < 100; ++i) {
            book.addOrder(Order(nextId + i, 100.0 + i*0.01, 10, true, OrderType::LIMIT));
        }
        int idToCancel = nextId + 50; 
        state.ResumeTiming();  
        book.cancelOrder(idToCancel);
        nextId += 100;  
    }
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_CancelOrder);

static void BM_ModifyOrder(benchmark::State& state) {
    int nextId = 0;
    for (auto _ : state) {
        state.PauseTiming();
        OrderBook book;
        for (int i = 0; i < 100; ++i) {
            book.addOrder(Order(nextId + i, 100.0 + i*0.01, 10, true, OrderType::LIMIT));
        }
        int idToModify = nextId + 50;
        state.ResumeTiming();
        book.modifyOrder(idToModify, 101.0, 20);  
        nextId += 100;
    }
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_ModifyOrder);

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

static void BM_BestAsk(benchmark::State& state) {
    OrderBook book;
    for (int i = 0; i < 1000; ++i) {
        book.addOrder(Order(i, 100.0 + i*0.01, 10, false, OrderType::LIMIT));
    }

    for (auto _ : state) {
        benchmark::DoNotOptimize(book.bestAsk());
    }
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_BestAsk);

static void BM_GetSpread(benchmark::State& state) {
    OrderBook book;
    for (int i = 0; i < 500; ++i) {
        book.addOrder(Order(i, 99.50 + i*0.001, 10, true, OrderType::LIMIT));
    }
    for (int i = 500; i < 1000; ++i) {
        book.addOrder(Order(i, 100.10 + (i-500)*0.001, 10, false, OrderType::LIMIT));
    }
    for (auto _ : state) {
        benchmark::DoNotOptimize(book.getSpread());
    }
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_GetSpread);

static void BM_GetMidPrice(benchmark::State& state) {
    OrderBook book;
    for (int i = 0; i < 500; ++i) {
        book.addOrder(Order(i, 99.50 + i*0.001, 10, true, OrderType::LIMIT));
    }
    for (int i = 500; i < 1000; ++i) {
        book.addOrder(Order(i, 100.10 + (i-500)*0.001, 10, false, OrderType::LIMIT));
    }
    for (auto _ : state) {
        benchmark::DoNotOptimize(book.getMidPrice());
    }
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_GetMidPrice);

static void BM_GetVolumeInfo(benchmark::State& state) {
    OrderBook book;
    for (int i = 0; i < 500; ++i) {
        book.addOrder(Order(i, 100.0 - i*0.01, 10, true, OrderType::LIMIT));
    }
    for (int i = 500; i < 1000; ++i) {
        book.addOrder(Order(i, 100.0 + (i-500)*0.01, 10, false, OrderType::LIMIT));
    }
    for (auto _ : state) {
        benchmark::DoNotOptimize(book.getVolumeInfo());
    }
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_GetVolumeInfo);

static void BM_GetVWAP(benchmark::State& state) {
    OrderBook book;
    for (int i = 0; i < 100; ++i) {
        book.addOrder(Order(i, 100.0, 10, true, OrderType::LIMIT));
    }
    for (int i = 100; i < 200; ++i) {
        book.addOrder(Order(i, 100.0, 10, false, OrderType::LIMIT));
    }
    for (auto _ : state) {
        benchmark::DoNotOptimize(book.getVWAP());
    }
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_GetVWAP);

static void BM_GetRecentTrades(benchmark::State& state) {
    OrderBook book;
    for (int i = 0; i < 1000; ++i) {
        book.addOrder(Order(i*2, 100.0, 10, true, OrderType::LIMIT));
        book.addOrder(Order(i*2+1, 100.0, 10, false, OrderType::LIMIT));
    }
    for (auto _ : state) {
        benchmark::DoNotOptimize(book.getRecentTrades(10));
    }
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_GetRecentTrades);

BENCHMARK_MAIN();