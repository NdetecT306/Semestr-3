// monitor.h
#ifndef MONITOR_H
#define MONITOR_H
#include <atomic>
using namespace std;

extern atomic<long> ordersReceived;
extern atomic<long> ordersProcessed;
extern atomic<long> ordersWrittenToHash;
extern atomic<long> ordersWrittenToCSV;
void incrementOrdersReceived();
void incrementOrdersProcessed();
void incrementOrdersWrittenToHash();
void incrementOrdersWrittenToCSV();
long getOrdersReceived();
long getOrdersProcessed();
long getOrdersWrittenToHash();
long getOrdersWrittenToCSV();

#endif
