#include "commands.h"
#include "struct.h"
#include "CSV.h"
#include "Block.h"
#include "struct.h"
#include "workWithJson.h"
int getNextFileNumber(const string& tablePath) { //Номер файла csv
    int maxFile = 0;
    for (const auto& entry : fs::directory_iterator(tablePath)) {
        if (entry.is_regular_file()) {
            string filename = entry.path().filename().string();
            if (hasCsvExtension(filename)) {
                string numberStr = filename.substr(0, filename.find('.'));
                int fileNum = stoi(numberStr);
                if (fileNum > maxFile) maxFile = fileNum;
            }
        }
    }
    return maxFile + 1;
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
bool insertIntoCSV(const string& tableName, Vec& values) { //Вставка в csv
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
    int primaryKey = getNextPrimaryKey(tableName);
    int currentFileNum = 1;
    string currentCsvPath = tablePath + "/1.csv";
    for (const auto& entry : fs::directory_iterator(tablePath)) {
        if (entry.is_regular_file()) {
            string filename = entry.path().filename().string();
            if (hasCsvExtension(filename)) {
                string numberStr = filename.substr(0, filename.find('.'));
                int fileNum = stoi(numberStr);
                if (fileNum > currentFileNum) {
                    currentFileNum = fileNum;
                    currentCsvPath = tablePath + "/" + to_string(fileNum) + ".csv";
                }
            }
        }
    }
    int rowCount = getCurrentRowCount(currentCsvPath);
    if (rowCount >= tuplesLimit) {
        currentFileNum = getNextFileNumber(tablePath);
        currentCsvPath = tablePath + "/" + to_string(currentFileNum) + ".csv";
        ifstream oldFile(tablePath + "/1.csv");
        ofstream newFile(currentCsvPath);
        if (oldFile.is_open() && newFile.is_open()) {
            string header;
            getline(oldFile, header);
            newFile << header << endl;
            oldFile.close();
            newFile.close();
        }
    }
    ofstream file(currentCsvPath, ios::app);
    if (!file.is_open()) {
        cerr << "Не удалось открыть файл " << currentCsvPath << " для записи" << endl;
        unlockTable(tableName);
        return false;
    }
    file << primaryKey;
    for (size_t i = 0; i < values.getSize(); i++) {
        file << "," << values[i];
    }
    file << endl;
    file.close();
    success = true;
    
    unlockTable(tableName);
    return success;
}
bool deleteFromCSV(const string& tableName, const string& columnName, const string& value) { //Удалить из csv
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
            if (!inputFile.is_open() || !outputFile.is_open()) {
                continue;
            }
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
                if (targetColumnIndex < rowValues.getSize() && 
                    rowValues[targetColumnIndex] == value) {
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
