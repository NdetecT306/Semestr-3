#ifndef LOCK_H
#define LOCK_H

#include <string>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <thread>
#include <chrono>
#include <shared_mutex> //Прекраснейшая вещь для асинхронного сервера. Обеспечивает разделение доступа к общим данным между несколькими потоками.
#include <mutex>
using namespace std;
//Позволяет нескольким потокам одновременно читать общий ресурс, но гарантирует, что только один поток может писать в этот ресурс в любое время. 
class ReadWriteLock {
private:
    mutable shared_mutex mtx;
public:
    shared_lock<shared_mutex> read_lock() const {
        return shared_lock<shared_mutex>(mtx);
    }
    unique_lock<shared_mutex> write_lock() {
        return unique_lock<shared_mutex>(mtx);
    }
    void lock_read() { mtx.lock_shared(); }
    void unlock_read() { mtx.unlock_shared(); }
    void lock_write() { mtx.lock(); }
    void unlock_write() { mtx.unlock(); }
};
bool lockTable(const string& tableName);
bool unlockTable(const string& tableName);
bool isTableLocked(const string& tableName);
bool waitForTableUnlock(const string& tableName, int maxWaitMs);

#endif
