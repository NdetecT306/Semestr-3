#include "lock.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <thread>
#include <chrono>
using namespace std;
namespace fs = std::filesystem;
extern string currentSchemaName;
bool lockTable(const string& tableName) { //Блокирует таблицу путем создания файла блокировки
    string lockFilePath = currentSchemaName + "/" + tableName + "/" + tableName + "_lock";
    ofstream lockFile(lockFilePath);
    if (!lockFile.is_open()) {
        cerr << "Не удалось создать файл блокировки для таблицы '" << tableName << "'" << endl;
        return false;
    }
    lockFile << "locked" << endl;
    lockFile.close();
    return true;
}
bool unlockTable(const string& tableName) { //Снимает блокировку путем удаления файла 
    string lockFilePath = currentSchemaName + "/" + tableName + "/" + tableName + "_lock";
    if (fs::exists(lockFilePath)) {
        return fs::remove(lockFilePath);
    }
    return true;
}
bool isTableLocked(const string& tableName) { //Логично из названия -  проверяет, открыта ли таблица
    string lockFilePath = currentSchemaName + "/" + tableName + "/" + tableName + "_lock";
    return fs::exists(lockFilePath);
}
bool waitForTableUnlock(const string& tableName, int maxWaitMs) { //ждем время по талончику (waitInterval)
    int waited = 0;
    int waitInterval = 100; 
    while (isTableLocked(tableName) && waited < maxWaitMs) {
        this_thread::sleep_for(chrono::milliseconds(waitInterval));
        waited += waitInterval;
    }
    return !isTableLocked(tableName);
}
