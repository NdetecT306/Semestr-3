#ifndef TABLE_LOCKS_H
#define TABLE_LOCKS_H
#include <string>
using namespace std;
bool lockTable(const string& tableName);
bool unlockTable(const string& tableName);
bool isTableLocked(const string& tableName);
bool waitForTableUnlock(const string& tableName, int maxWaitMs = 5000);
#endif 
