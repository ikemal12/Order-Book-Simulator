CXX = g++
CXXFLAGS = -std=c++20 -Iinclude 
LDFLAGS = -lstdc++exp 

TARGET = orderbook 

all: $(TARGET)

$(TARGET): src/main.cpp src/orderbook.cpp 
	$(CXX) $(CXXFLAGS) src/main.cpp src/orderbook.cpp -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(TARGET)

run: all
	./$(TARGET)

bench: benchmark/benchmark.cpp src/orderbook.cpp 
	$(CXX) $(CXXFLAGS) benchmark/benchmark.cpp src/orderbook.cpp -o bench $(LDFLAGS)

.PHONY: all clean run bench

BENCHMARK_DIR = $(HOME)/Dev/google-benchmark
CXXFLAGS += -I$(BENCHMARK_DIR)/include
LDFLAGS += -L$(BENCHMARK_DIR)/src -lbenchmark -lpthread