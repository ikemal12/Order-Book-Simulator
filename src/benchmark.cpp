#include "orderbook.h"
#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <random>
#include <format>
#include <fstream>
#include <ctime>

void addRandomOrders(OrderBook& book, int numOrders, int threadId) {
    std::random_device rd;
    std::mt19937 gen(rd() + threadId);
    
    std::uniform_real_distribution<> priceDist(95.0, 105.0);
    std::uniform_int_distribution<> quantityDist(1, 100);
    std::uniform_int_distribution<> sideDist(0, 1);

    int baseId = threadId * 100000;

    for (int i = 0; i < numOrders; ++i) {
        double price = priceDist(gen);
        int quantity = quantityDist(gen);
        bool isBuy = sideDist(gen) == 1;

        Order order(baseId + i, price, quantity, isBuy, OrderType::LIMIT);
        book.addOrder(order);
    }
}

double runBenchmark(int numThreads, int ordersPerThread) {
    OrderBook book;

    auto start = std::chrono::high_resolution_clock::now();

    if (numThreads == 1) {
        addRandomOrders(book, ordersPerThread, 0);
    } else {
        std::vector<std::thread> threads;

        for (int i = 0; i < numThreads; ++i) {
            threads.emplace_back(addRandomOrders, std::ref(book), ordersPerThread, i);
        }

        for (auto& t : threads) {
            t.join();
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    return elapsed.count();
}

int main() {
    const int ORDERS_PER_THREAD = 10000;
    const int RUNS_PER_CONFIG = 3;

    std::ofstream logFile("benchmark/results.txt", std::ios::app);
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);

    logFile << "\n=== Benchmark Run: " << std::ctime(&time_t_now) << "===\n";
    logFile << "Orders per thread: " << ORDERS_PER_THREAD << "\n";
    logFile << "Runs per configuration: " << RUNS_PER_CONFIG << "\n\n";

    std::vector<int> threadCounts = {1, 2, 4, 8};

    double baselineTime = 0.0;

    for (int numThreads : threadCounts) {
        double totalTime = 0.0;

        // run multiple times and average
        for (int run = 0; run < RUNS_PER_CONFIG; ++run) {
            double time = runBenchmark(numThreads, ORDERS_PER_THREAD);
            totalTime += time;
        }

        double avgTime = totalTime / RUNS_PER_CONFIG;
        int totalOrders = numThreads * ORDERS_PER_THREAD;
        double ordersPerSecond = totalOrders / avgTime;

        if (numThreads == 1) {
            baselineTime = avgTime;
        }

        double speedup = baselineTime / avgTime;

        std::cout << std::format("Threads: {} | Orders: {:>5} | Time: {:.3f}s | Throughput: {:>8.0f} orders/sec | Speedup: {:.2f}x\n",
                                 numThreads, totalOrders, avgTime, ordersPerSecond, speedup);
        
        logFile << std::format("Threads: {} | Orders: {:>5} | Time: {:.3f}s | Throughput: {:>8.0f} orders/sec | Speedup: {:.2f}x\n",
                               numThreads, totalOrders, avgTime, ordersPerSecond, speedup);
    }
    logFile << "\n";
    logFile.close();
}