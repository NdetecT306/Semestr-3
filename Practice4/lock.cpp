#include "lock.h"
namespace fs = std::filesystem;
extern string currentSchemaName;
bool lockTable(const string& tableName) {
    string lockFilePath = currentSchemaName + "/" + tableName + "/" + tableName + "_lock";
    if (fs::exists(lockFilePath)) {
        return false;  // Уже заблокирована
    }
    ofstream lockFile(lockFilePath);
    if (!lockFile.is_open()) {
        cerr << "Не удалось создать файл блокировки для таблицы '" << tableName << "'" << endl;
        return false;
    }
    lockFile << "locked" << endl;
    lockFile.close();
    return true;
}
bool unlockTable(const string& tableName) {
    string lockFilePath = currentSchemaName + "/" + tableName + "/" + tableName + "_lock";
    if (fs::exists(lockFilePath)) {
        return fs::remove(lockFilePath);
    }
    return true;
}
bool isTableLocked(const string& tableName) {
    string lockFilePath = currentSchemaName + "/" + tableName + "/" + tableName + "_lock";
    return fs::exists(lockFilePath);
}
bool waitForTableUnlock(const string& tableName, int maxWaitMs) {
    int waited = 0;
    int waitInterval = 2;  // Увеличить с 10
    while (isTableLocked(tableName) && waited < maxWaitMs) {
        this_thread::sleep_for(chrono::milliseconds(waitInterval));
        waited += waitInterval;
        if (waited > maxWaitMs / 2) {
            cout << "Долгое ожидание разблокировки таблицы " << tableName << " (" << waited << " мс)" << endl;
        }
    }
    return !isTableLocked(tableName);
}
