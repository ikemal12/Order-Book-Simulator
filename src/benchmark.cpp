#include "orderbook.h"
#include <iostream>
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

double runBenchmark(int numOperations) {
    OrderBook book;

    // pre-populate
    for (int i = 0; i < 100; ++i) {
        book.addOrder(Order(i, 100.0 + (i % 10), 10, i % 2 == 0, OrderType::LIMIT));
    }

    auto start = std::chrono::high_resolution_clock::now();
    
    mixedWorkload(book, numOperations, 0);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    return elapsed.count();
}

int main() {
    std::vector<int> OPERATION_COUNTS = {1000, 10000, 100000, 1000000};
    const int RUNS_PER_CONFIG = 3;

    std::ofstream logFile("benchmark/results.txt", std::ios::app);
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);

    logFile << "\n=== Benchmark Run: " << std::ctime(&time_t_now) << "===\n";
    logFile << "Runs per configuration: " << RUNS_PER_CONFIG << "\n\n";

    double baselineTime = 0.0;

    for (int numOperations : OPERATION_COUNTS) {
        double totalTime = 0.0;

        // run multiple times and average
        for (int run = 0; run < RUNS_PER_CONFIG; ++run) {
            double time = runBenchmark(numOperations);
            totalTime += time;
        }

        double avgTime = totalTime / RUNS_PER_CONFIG;
        double operationsPerSecond = numOperations / avgTime;

        std::cout << std::format("Operations: {:>7} | Time: {:.4f}s | Throughput: {:>10.0f} operations/sec\n",
                                 numOperations, avgTime, operationsPerSecond);
        
        logFile << std::format("Operations: {:>7} | Time: {:.4f}s | Throughput: {:>10.0f} operations/sec\n",
                               numOperations, avgTime, operationsPerSecond);
    }
    logFile << "\n";
    logFile.close();
}