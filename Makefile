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

benchmark: src/benchmark.cpp src/orderbook.cpp 
	$(CXX) $(CXXFLAGS) src/benchmark.cpp src/orderbook.cpp -o benchmark $(LDFLAGS)

.PHONY: all clean run benchmark