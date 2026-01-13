// monitor.cpp
#include "monitor.h"
#include <iostream>
#include <atomic>
#include <thread>
#include <chrono>
#include <algorithm>
using namespace std;

atomic<long> ordersReceived{0};
atomic<long> ordersProcessed{0};
atomic<long> ordersWrittenToHash{0};
atomic<long> ordersWrittenToCSV{0};
// Функции для инкремента
void incrementOrdersReceived() { 
    ordersReceived.fetch_add(1, memory_order_relaxed); 
}
void incrementOrdersProcessed() { 
    ordersProcessed.fetch_add(1, memory_order_relaxed); 
}
void incrementOrdersWrittenToHash() { 
    ordersWrittenToHash.fetch_add(1, memory_order_relaxed); 
}
void incrementOrdersWrittenToCSV() { 
    ordersWrittenToCSV.fetch_add(1, memory_order_relaxed); 
}
template<typename T>
double safeDivide(T numerator, T denominator) {
    if (denominator == 0) return 0.0;
    return static_cast<double>(numerator) / static_cast<double>(denominator) * 100.0;
}
