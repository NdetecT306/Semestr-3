#include "MYCSV.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>
#include "json.hpp"
#include "struct.h"
#include "lock.h"
#include "command.h"  
using namespace std;
using json = nlohmann::json;
namespace fs = std::filesystem;
extern string currentSchemaName;
extern json schemaConfig;
bool hasCsvExtension(const string& filename) { //Нащ CSV
    if (filename.length() < 4) return false;
    return filename.substr(filename.length() - 4) == ".csv";
}
int getNextPrimaryKey(const string& tableName) { //Получить первичный ключ. После каждого добавления он +1
    string seqFilePath = currentSchemaName + "/" + tableName + "/" + tableName + "_pk_sequence";
    int currentKey = 0;
    string lockFile = seqFilePath + ".lock";
    ofstream lock(lockFile);
    if (fs::exists(seqFilePath)) {
        ifstream seqFile(seqFilePath);
        if (seqFile.is_open()) {
            string line;
            if (getline(seqFile, line)) {
                bool isNumber = true;
                for (char c : line) {
                    if (!isdigit(c)) {
                        isNumber = false;
                        break;
                    }
                }
                if (isNumber && !line.empty()) {
                    currentKey = stoi(line);
                }
            }
            seqFile.close();
        }
    }
    int nextKey = currentKey + 1;
    ofstream seqFile(seqFilePath);
    if (seqFile.is_open()) {
        seqFile << nextKey << endl;
        seqFile.close();
    }
    lock.close();
    fs::remove(lockFile);
    return nextKey;
}
bool createTableDirectory(const string& tableName) { //Создаем CSV в директории
    string tablePath = currentSchemaName + "/" + tableName;
    if (!fs::exists(tablePath)) {
        fs::create_directory(tablePath);
    }
    string seqFilePath = tablePath + "/" + tableName + "_pk_sequence"; //Колонка с pk
    if (!fs::exists(seqFilePath)) {
        ofstream seqFile(seqFilePath);
        if (seqFile.is_open()) {
            seqFile << "0" << endl;
            seqFile.close();
        } else {
            cerr << "Не удалось создать файл последовательности: " << seqFilePath << endl;
            return false;
        }
    }
    string csvFilePath = tablePath + "/1.csv";
    if (!fs::exists(csvFilePath)) {
        ofstream csvFile(csvFilePath);
        if (csvFile.is_open()) {
            csvFile << tableName + "_pk";
            if (schemaConfig.contains("structure") && schemaConfig["structure"].contains(tableName)) {
                auto columns = schemaConfig["structure"][tableName];
                for (size_t i = 0; i < columns.size(); i++) {
                    csvFile << "," << columns[i];
                }
            }
            csvFile << endl;
            csvFile.close();
        } else {
            cerr << "Не удалось создать CSV файл: " << csvFilePath << endl;
            return false;
        }
    }
    return true;
}
bool createDirectory() { //Наш mkdir со всеми структурами
    if (currentSchemaName.empty()) {
        cerr << "Имя схемы не установлено." << endl;
        return false;
    }
    if (!fs::exists(currentSchemaName)) {
        if (!fs::create_directory(currentSchemaName)) {
            cerr << "Не удалось создать директорию схемы: " << currentSchemaName << endl;
            return false;
        }
    }
    if (schemaConfig.contains("structure")) {
        auto structure = schemaConfig["structure"];
        for (auto it = structure.begin(); it != structure.end(); ++it) {
            string tableName = it.key();
            if (!createTableDirectory(tableName)) {
                cerr << "Не удалось создать директорию для таблицы: " << tableName << endl;
                return false;
            }
        }
    }
    return true;
}
Vec getTableDataFromHash(const string& tableName) { //Извлекает данные из хэш-таблицы для указанной таблицы
    Vec result;
    DOUBLE_HASH* tableHash = GET_SCHEMA(tableName);
    if (tableHash == nullptr) {
        cerr << "Таблица '" << tableName << "' не найдена в SCHEMA_HASH" << endl;
        return result;
    }
    for (int i = 0; i < HASH_SIZE; i++) {
        if (tableHash[i].isValid()) {
            Vec& values = tableHash[i].getValue();
            for (size_t j = 0; j < values.getSize(); j++) {
                result.PUSH(values[j]);
            }
        }
    }
    return result;
}
int getColumnIndex(const string& tableName, const string& columnName) { //Поиск нужной олони\ки
    if (!schemaConfig["structure"].contains(tableName)) {
        cerr << "Таблица '" << tableName << "' не найдена в схеме" << endl;
        return -1;
    }
    auto columns = schemaConfig["structure"][tableName];
    for (size_t i = 0; i < columns.size(); i++) {
        if (columns[i] == columnName) {
            return i;
        }
    }
    cerr << "Колонка '" << columnName << "' не найдена в таблице '" << tableName << "'" << endl;
    return -1;
}
void loadAllDataToHash() { //По порядку десериализация
    if (!schemaConfig.contains("structure")) return;
    auto structure = schemaConfig["structure"];
    for (auto it = structure.begin(); it != structure.end(); ++it) {
        string tableName = it.key();
        DOUBLE_HASH* tableHash = new DOUBLE_HASH[HASH_SIZE];
        string tablePath = currentSchemaName + "/" + tableName;
        if (!fs::exists(tablePath)) continue;
        Vec csvFiles;
        for (const auto& entry : fs::directory_iterator(tablePath)) {
            if (entry.is_regular_file()) {
                string filename = entry.path().filename().string();
                if (hasCsvExtension(filename)) {
                    csvFiles.PUSH(entry.path().string());
                }
            }
        }
        for (size_t i = 0; i < csvFiles.getSize(); i++) {
            for (size_t j = i + 1; j < csvFiles.getSize(); j++) {
                string file1 = fs::path(csvFiles[i]).filename().string();
                string file2 = fs::path(csvFiles[j]).filename().string();
                int num1 = stoi(file1.substr(0, file1.find('.')));
                int num2 = stoi(file2.substr(0, file2.find('.')));
                if (num1 > num2) {
                    string temp = csvFiles[i];
                    csvFiles[i] = csvFiles[j];
                    csvFiles[j] = temp;
                }
            }
        }
        for (size_t i = 0; i < csvFiles.getSize(); i++) {
            ifstream file(csvFiles[i]);
            if (!file.is_open()) continue;
            string line;
            bool first = true;
            while (getline(file, line)) {
                if (first) {
                    first = false;
                    continue;
                }
                if (!line.empty()) {
                    stringstream ss(line);
                    string primaryKeyStr;
                    if (getline(ss, primaryKeyStr, ',')) {
                        bool isNumber = true;
                        for (char c : primaryKeyStr) {
                            if (!isdigit(c)) {
                                isNumber = false;
                                break;
                            }
                        }
                        if (isNumber && !primaryKeyStr.empty()) {
                            int primaryKey = stoi(primaryKeyStr);
                            Vec value;
                            value.PUSH(line);
                            
                            int ind = hash1(primaryKey);
                            int step = hash2(primaryKey);
                            int startInd = ind;
                            int trials = 0;
                            
                            while (trials < HASH_SIZE) {
                                if (tableHash[ind].getIsEmpty() || tableHash[ind].getIsDeleted()) {
                                    tableHash[ind].init(primaryKey, value);
                                    break;
                                }
                                ind = (ind + step) % HASH_SIZE;
                                trials++;
                                if (ind == startInd) break;
                            }
                        }
                    }
                }
            }
            file.close();
        }
        ADD_SCHEMA(tableName, tableHash);
    }
    cout << "Все данные загружены в SCHEMA_HASH." << endl;
}
int getCurrentRowCount(const string& csvFilePath) {
    ifstream file(csvFilePath);
    if (!file.is_open()) return 0;
    int rowCount = -1;
    string line;
    while (getline(file, line)) {
        rowCount++;
    }
    file.close();
    return max(0, rowCount);
}
int getTuplesLimit() { //Вернуть tuple_limits
    if (schemaConfig.contains("tuples_limit")) {
        return schemaConfig["tuples_limit"];
    }
    return 1000;
}
int insertIntoCSV(const string& tableName, Vec& values) { //Вставка и удаления в csv
    string tablePath = currentSchemaName + "/" + tableName;
    if (!fs::exists(tablePath)) {
        cerr << "Таблица '" << tableName << "' не существует" << endl;
        return -1;
    }
    if (!waitForTableUnlock(tableName, 5000)) {
        cerr << "Таблица '" << tableName << "' заблокирована, операция отменена" << endl;
        return -1;
    }
    if (!lockTable(tableName)) {
        cerr << "Не удалось заблокировать таблицу '" << tableName << "'" << endl;
        return -1;
    }
    int primaryKey = getNextPrimaryKey(tableName);
    int tuplesLimit = getTuplesLimit();
    int currentFileNum = 1;
    string currentCsvPath = tablePath + "/1.csv";
    int maxFileNum = 1;
    for (const auto& entry : fs::directory_iterator(tablePath)) {
        if (entry.is_regular_file()) {
            string filename = entry.path().filename().string();
            if (hasCsvExtension(filename)) {
                string numberStr = filename.substr(0, filename.find('.'));
                int fileNum = stoi(numberStr);
                if (fileNum > maxFileNum) {
                    maxFileNum = fileNum;
                }
            }
        }
    }
    currentFileNum = maxFileNum;
    currentCsvPath = tablePath + "/" + to_string(currentFileNum) + ".csv";
    int currentRowCount = getCurrentRowCount(currentCsvPath);
    if (currentRowCount >= tuplesLimit) {
        currentFileNum = maxFileNum + 1;
        currentCsvPath = tablePath + "/" + to_string(currentFileNum) + ".csv";
        ofstream newFile(currentCsvPath);
        if (newFile.is_open()) {
            string firstFilePath = tablePath + "/1.csv";
            ifstream firstFile(firstFilePath);
            if (firstFile.is_open()) {
                string header;
                if (getline(firstFile, header)) {
                    newFile << header << endl;
                }
                firstFile.close();
            }
            newFile.close();
        } else {
            cerr << "Не удалось создать новый файл: " << currentCsvPath << endl;
            unlockTable(tableName);
            return -1;
        }
    }
    ofstream file(currentCsvPath, ios::app);
    if (!file.is_open()) {
        cerr << "Не удалось открыть файл " << currentCsvPath << " для записи" << endl;
        unlockTable(tableName);
        return -1;
    }
    file << primaryKey;
    for (size_t i = 0; i < values.getSize(); i++) {
        file << "," << values[i];
    }
    file << endl;
    file.close();
    unlockTable(tableName);
    return primaryKey;
}
bool deleteFromCSV(const string& tableName, const string& columnName, const string& value, Vec& deletedPrimaryKeys) {
    string tablePath = currentSchemaName + "/" + tableName;
    if (!fs::exists(tablePath)) {
        cerr << "Таблица '" << tableName << "' не существует" << endl;
        return false;
    }
    if (!waitForTableUnlock(tableName, 5000)) {
        cerr << "Таблица '" << tableName << "' заблокирована, операция отменена" << endl;
        return false;
    }
    if (!lockTable(tableName)) {
        cerr << "Не удалось заблокировать таблицу '" << tableName << "'" << endl;
        return false;
    }
    bool success = false;
    if (!schemaConfig["structure"].contains(tableName)) {
        cerr << "Таблица '" << tableName << "' не найдена в схеме" << endl;
        unlockTable(tableName);
        return false;
    }
    auto columns = schemaConfig["structure"][tableName];
    int targetColumnIndex = -1;
    for (size_t i = 0; i < columns.size(); i++) {
        if (columns[i] == columnName) {
            targetColumnIndex = i + 1;
            break;
        }
    }
    if (targetColumnIndex == -1) {
        cerr << "Ошибка: колонка '" << columnName << "' не найдена в таблице '" << tableName << "'" << endl;
        unlockTable(tableName);
        return false;
    }
    bool deleted = false;
    for (const auto& entry : fs::directory_iterator(tablePath)) {
        if (entry.is_regular_file() && hasCsvExtension(entry.path().string())) {
            string csvFilePath = entry.path().string();
            string tempFilePath = csvFilePath + ".tmp";
            ifstream inputFile(csvFilePath);
            ofstream outputFile(tempFilePath);
            if (!inputFile.is_open() || !outputFile.is_open()) continue; 
            string line;
            bool isFirstLine = true;
            while (getline(inputFile, line)) {
                if (isFirstLine) {
                    outputFile << line << endl;
                    isFirstLine = false;
                    continue;
                }
                Vec rowValues;
                stringstream ss(line);
                string cell;
                while (getline(ss, cell, ',')) {
                    rowValues.PUSH(cell);
                }
                string primaryKey = rowValues[0];
                if (targetColumnIndex < rowValues.getSize() && 
                    rowValues[targetColumnIndex] == value) {
                    deletedPrimaryKeys.PUSH(primaryKey);
                    deleted = true;
                    continue;
                }
                for (size_t i = 0; i < rowValues.getSize(); i++) {
                    if (i > 0) outputFile << ",";
                    outputFile << rowValues[i];
                }
                outputFile << endl;
            }
            inputFile.close();
            outputFile.close();
            fs::remove(csvFilePath);
            fs::rename(tempFilePath, csvFilePath);
        }
    }
    success = deleted;
    unlockTable(tableName);
    return success;
}
// Вычисляет декартово произведение таблиц с применением условий WHERE
void cartesianProductFromHash(const Vec& tables, const Vec& columns, const string& whereCond, Vec& result) {
    if (tables.empty()) return;
    Vec currentCombination;
    processCombination(0, currentCombination, tables, columns, whereCond, result);
}
