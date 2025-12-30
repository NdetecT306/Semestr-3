#ifndef LOCK_H
#define LOCK_H

#include <string>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <thread>
#include <chrono>
using namespace std;

bool lockTable(const string& tableName);
bool unlockTable(const string& tableName);
bool isTableLocked(const string& tableName);
bool waitForTableUnlock(const string& tableName, int maxWaitMs = 5000);


#endif
