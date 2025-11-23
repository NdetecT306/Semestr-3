#include <string>
using namespace std;
// Функции управления блокировками таблиц
bool lockTable(const string& tableName);
bool unlockTable(const string& tableName);
bool isTableLocked(const string& tableName);
bool waitForTableUnlock(const string& tableName, int maxWaitMs = 5000);

