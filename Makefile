CXX = g++
CXXFLAGS = -std=c++20 -Iinclude 
LDFLAGS = -lstdc++exp 

TARGET = orderbook 

BENCHMARK_DIR = $(HOME)/Dev/google-benchmark
BENCHMARK_LIB = $(BENCHMARK_DIR)/src/libbenchmark.a
CXXFLAGS += -I$(BENCHMARK_DIR)/include -DBENCHMARK_STATIC_DEFINE

all: $(TARGET)

$(TARGET): src/main.cpp src/orderbook.cpp 
	$(CXX) $(CXXFLAGS) src/main.cpp src/orderbook.cpp -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(TARGET)

run: all
	./$(TARGET)

bench: benchmark/benchmark.cpp src/orderbook.cpp 
	$(CXX) $(CXXFLAGS) benchmark/benchmark.cpp src/orderbook.cpp -o bench $(LDFLAGS)

google-bench: benchmark/google_benchmark.cpp src/orderbook.cpp
	$(CXX) $(CXXFLAGS) benchmark/google_benchmark.cpp src/orderbook.cpp -o google-bench $(BENCHMARK_LIB) -lpthread -lstdc++exp -lshlwapi

.PHONY: all clean run bench google-bench