#include "commands.h"
#include "struct.h"
#include "CSV.h"
#include "Block.h"
#include "struct.h"
#include "workWithJson.h"
bool hasCsvExtension(const string& filename) { //Для .cvs
    if (filename.length() < 4) return false;
    return filename.substr(filename.length() - 4) == ".csv";
}
bool lockTable(const string& tableName) { //Блокировка
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
bool waitForTableUnlock(const string& tableName, int maxWaitMs = 5000) {
    int waited = 0;
    int waitInterval = 100; 
    while (isTableLocked(tableName) && waited < maxWaitMs) {
        this_thread::sleep_for(chrono::milliseconds(waitInterval));
        waited += waitInterval;
    }
    return !isTableLocked(tableName);
}
int getNextPrimaryKey(const string& tableName) {// Функции для работы с первичными ключами
    string seqFilePath = currentSchemaName + "/" + tableName + "/" + tableName + "_pk_sequence";
    int ourKey = 1;
    if (fs::exists(seqFilePath)) {
        ifstream seqFile(seqFilePath);
        if (seqFile.is_open()) {
            string line;
            if (getline(seqFile, line)) {
                try {
                    ourKey = stoi(line) + 1;
                } catch (...) {
                    ourKey = 1;
                }
            }
            seqFile.close();
        }
    }
    ofstream seqFile(seqFilePath);
    if (seqFile.is_open()) {
        seqFile << ourKey << endl;
        seqFile.close();
    }
    return ourKey;
}
bool createDirectory() {// Функция mkdir
    if (currentSchemaName.empty()) {
        cerr << "Имя схемы не установлено." << endl;
        return false;
    }
    if (!fs::exists(currentSchemaName)) {
        fs::create_directory(currentSchemaName);
    }
    if (schemaConfig.contains("structure")) {
        auto structure = schemaConfig["structure"];
        for (auto it = structure.begin(); it != structure.end(); ++it) {
            string tableName = it.key();
            string tablePath = currentSchemaName + "/" + tableName;
            if (!fs::exists(tablePath)) {
                fs::create_directory(tablePath);
            }
            string seqFilePath = tablePath + "/" + tableName + "_pk_sequence";
            if (!fs::exists(seqFilePath)) {
                ofstream seqFile(seqFilePath);
                if (seqFile.is_open()) {
                    seqFile << "0" << endl;
                    seqFile.close();
                }
            }
            string csvFilePath = tablePath + "/1.csv";
            if (!fs::exists(csvFilePath)) {
                ofstream csvFile(csvFilePath);
                if (csvFile.is_open()) {
                    csvFile << tableName + "_pk";
                    auto columns = it.value();
                    for (size_t i = 0; i < columns.size(); i++) {
                        csvFile << "," << columns[i];
                    }
                    csvFile << endl;
                    csvFile.close();
                }
            }
        }
    }
    return true;
}
