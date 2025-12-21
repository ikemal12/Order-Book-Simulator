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

void mixedWorkload(OrderBook& book, int numOperations, int threadId) {
    std::random_device rd;
    std::mt19937 gen(rd() + threadId);

    std::uniform_real_distribution<> priceDist(95.0, 105.0);
    std::uniform_int_distribution<> quantityDist(1, 100);
    std::uniform_int_distribution<> sideDist(0, 1);
    std::uniform_int_distribution<> operationDist(1, 100);
    
    int baseId = threadId * 100000;

    for (int i = 0; i < numOperations; ++i) {
        if (i % 1000 == 0 && threadId == 0) {  // Only thread 0 prints
            std::cout << ".";  // Progress indicator
            std::cout.flush();
        }
        
        int operationType = operationDist(gen);

        if (operationType <= 20) {
            // 20% writes - add order
            Order order(baseId + i, priceDist(gen), quantityDist(gen), sideDist(gen) == 1, OrderType::LIMIT);
            book.addOrder(order);
        } else {
            // 80% reads
            if (operationType <= 40) {
                book.bestBid();
            } else if (operationType <= 60) {
                book.bestAsk();
            } else if (operationType <= 80) {
                book.getMidPrice();
            } else {
                book.getSpread();
            }
        }
    }
}

double runBenchmark(int numThreads, int operationsPerThread) {
    OrderBook book;

    // pre-populate
    for (int i = 0; i < 100; ++i) {
        book.addOrder(Order(i, 100.0 + (i % 10), 10, i % 2 == 0, OrderType::LIMIT));
    }

    auto start = std::chrono::high_resolution_clock::now();

    if (numThreads == 1) {
        //addRandomOrders(book, ordersPerThread, 0);
        mixedWorkload(book, operationsPerThread, 0);
    } else {
        std::vector<std::thread> threads;

        for (int i = 0; i < numThreads; ++i) {
            threads.emplace_back(mixedWorkload, std::ref(book), operationsPerThread, i);
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
    const int OPERATIONS_PER_THREAD = 1000;
    const int RUNS_PER_CONFIG = 3;

    std::ofstream logFile("benchmark/results.txt", std::ios::app);
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);

    logFile << "\n=== Benchmark Run: " << std::ctime(&time_t_now) << "===\n";
    logFile << "Operations per thread: " << OPERATIONS_PER_THREAD << "\n";
    logFile << "Runs per configuration: " << RUNS_PER_CONFIG << "\n\n";

    std::vector<int> threadCounts = {1, 2, 4, 8};

    double baselineTime = 0.0;

    for (int numThreads : threadCounts) {
        double totalTime = 0.0;

        // run multiple times and average
        for (int run = 0; run < RUNS_PER_CONFIG; ++run) {
            double time = runBenchmark(numThreads, OPERATIONS_PER_THREAD);
            totalTime += time;
        }

        double avgTime = totalTime / RUNS_PER_CONFIG;
        int totalOperations = numThreads * OPERATIONS_PER_THREAD;
        double operationsPerSecond = totalOperations / avgTime;

        if (numThreads == 1) {
            baselineTime = avgTime;
        }

        double speedup = baselineTime / avgTime;

        std::cout << std::format("Threads: {} | Operations: {:>5} | Time: {:.3f}s | Throughput: {:>8.0f} operations/sec | Speedup: {:.2f}x\n",
                                 numThreads, totalOperations, avgTime, operationsPerSecond, speedup);
        
        logFile << std::format("Threads: {} | Operations: {:>5} | Time: {:.3f}s | Throughput: {:>8.0f} operations/sec | Speedup: {:.2f}x\n",
                               numThreads, totalOperations, avgTime, operationsPerSecond, speedup);
    }
    logFile << "\n";
    logFile.close();
}