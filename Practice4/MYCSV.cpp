#include "MYCSV.h"
using json = nlohmann::json;
namespace fs = std::filesystem;
extern string currentSchemaName;
extern json schemaConfig;
void markTableForSync(const string& tableName);
bool hasCsvExtension(const string& filename) {
    if (filename.length() < 4) return false;
    return filename.substr(filename.length() - 4) == ".csv";
}
int getNextPrimaryKey(const string& tableName) {
    static mutex seqMutex;
    lock_guard<mutex> lock(seqMutex);
    string seqFilePath = currentSchemaName + "/" + tableName + "/" + tableName + "_pk_sequence";
    int currentKey = 0;
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
    } else {
        ofstream seqFile(seqFilePath);
        if (seqFile.is_open()) {
            seqFile << "0" << endl;
            seqFile.close();
        }
    }
    int nextKey = currentKey + 1;
    ofstream seqFile(seqFilePath);
    if (seqFile.is_open()) {
        seqFile << nextKey << endl;
        seqFile.close();
    } else {
        cerr << "Не удалось записать в файл последовательности: " << seqFilePath << endl;
        return -1;
    }
    return nextKey;
}
bool createTableDirectory(const string& tableName) {
    string tablePath = currentSchemaName + "/" + tableName;
    if (!fs::exists(tablePath)) {
        if (!fs::create_directories(tablePath)) {
            cerr << "Не удалось создать директорию таблицы: " << tablePath << endl;
            return false;
        }
    }
    if (tableName != "user_lot") {
        string seqFilePath = tablePath + "/" + tableName + "_pk_sequence";
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
    }
    string csvFilePath = tablePath + "/1.csv";
    if (!fs::exists(csvFilePath)) {
        ofstream csvFile(csvFilePath);
        if (csvFile.is_open()) {
            if (tableName == "user_lot") {
                csvFile << "user_id,lot_id,quantity" << endl;
            } else if (tableName == "lot") {
                csvFile << "lot_id,name" << endl;
            } else if (tableName == "pair") {
                csvFile << "pair_id,first_lot_id,second_lot_id" << endl;
            } else if (tableName == "user") {
                csvFile << "user_id,username,key" << endl;
            } else if (tableName == "order") {
                csvFile << "order_id,user_id,pair_id,quantity,price,type,closed" << endl;
            } else {
                if (schemaConfig.contains("structure") && schemaConfig["structure"].contains(tableName)) {
                    auto columns = schemaConfig["structure"][tableName];
                    csvFile << tableName + "_id";
                    for (size_t i = 0; i < columns.size(); i++) {
                        csvFile << "," << columns[i];
                    }
                    csvFile << endl;
                } else {
                    csvFile << tableName + "_id" << endl;
                }
            }
            csvFile.close();
            cout << "Создан файл таблицы '" << tableName << "': " << csvFilePath << endl;
        } else {
            cerr << "Не удалось создать CSV файл: " << csvFilePath << endl;
            return false;
        }
    }
    return true;
}
bool createDirectory() {
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
Vec getTableDataFromHash(const string& tableName) {
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
int getColumnIndex(const string& tableName, const string& columnName) {
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
void loadAllDataToHash() {
    if (!schemaConfig.contains("structure")) {
        cerr << "Схема не содержит структуры таблиц" << endl;
        return;
    }
    auto structure = schemaConfig["structure"];
    for (auto it = structure.begin(); it != structure.end(); ++it) {
        string tableName = it.key();
        DOUBLE_HASH* tableHash = new DOUBLE_HASH[HASH_SIZE];
        string tablePath = currentSchemaName + "/" + tableName;
        if (!fs::exists(tablePath)) {
            cout << "ДИРЕКТОРИИ НЕ СУЩЕСТВУЕТ" << endl;
            delete[] tableHash;
            continue;
        }
        
        Vec csvFiles;
        for (const auto& el : fs::directory_iterator(tablePath)) {
            if (el.is_regular_file()) {
                string filename = el.path().filename().string();
                if (hasCsvExtension(filename)) {
                    csvFiles.PUSH(el.path().string());
                }
            }
        }
        
        if (csvFiles.empty()) {
            cout << "Отсутствуют .csv" << endl;
            delete[] tableHash;
            continue;
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
        int loadedRows = 0;
        for (size_t i = 0; i < csvFiles.getSize(); i++) {
            ifstream file(csvFiles[i]);
            if (!file.is_open()) {
                cerr << "Не удалось открыть файл: " << csvFiles[i] << endl;
                continue;
            }
            string line;
            bool first = true;
            while (getline(file, line)) {
                if (first) {
                    first = false;
                    continue;
                }
                if (line.empty()) {
                    continue;
                }
                Vec rowValues;
                stringstream ss(line);
                string cell;
                while (getline(ss, cell, ',')) {
                    rowValues.PUSH(cell);
                }
                if (rowValues.empty()) {
                    continue;
                }
                int primaryKey = -1;
                bool useAutoKey = false;
                if (tableName == "user_lot") {
                    if (rowValues.getSize() >= 2) {
                        try {
                            int userId = stoi(rowValues[0]);
                            int lotId = stoi(rowValues[1]);
                            primaryKey = userId * 1000 + lotId;
                        } catch (...) {
                            primaryKey = std::hash<string>{}(line) % HASH_SIZE;
                        }
                    } else {
                        useAutoKey = true;
                    }
                } else {
                    try {
                        primaryKey = stoi(rowValues[0]);
                    } catch (const exception& e) {
                        cerr << "Ошибка преобразования ключа в таблице '" << tableName  << "': " << rowValues[0] << " - " << e.what() << endl;
                        continue;
                    }
                }
                if (primaryKey == -1 || useAutoKey) {
                    primaryKey = loadedRows + 1;
                }
                Vec hashValue;
                hashValue.PUSH(line);
                int ind = hash1(primaryKey);
                int step = hash2(primaryKey);
                int startInd = ind;
                int trials = 0;
                bool inserted = false;
                
                while (trials < HASH_SIZE) {
                    if (tableHash[ind].getIsEmpty() || tableHash[ind].getIsDeleted()) {
                        tableHash[ind].init(primaryKey, hashValue);
                        loadedRows++;
                        inserted = true;
                        break;
                    }
                    ind = (ind + step) % HASH_SIZE;
                    trials++;
                    if (ind == startInd) break;
                }
                if (!inserted) {
                    cerr << "Не удалось вставить запись в хэш: " << line << endl;
                }
            }
            file.close();
        }
        if (!ADD_SCHEMA(tableName, tableHash)) {
            cerr << "Не удалось добавить таблицу '" << tableName << "' в SCHEMA_HASH" << endl;
            delete[] tableHash;
        } else {
            markTableForSync(tableName);
        }
    }
    vector<string> tablesToCheck = {"user", "lot", "user_lot", "pair", "order"};
    for (const auto& tableName : tablesToCheck) {
        DOUBLE_HASH* hash = GET_SCHEMA(tableName);
        if (hash != nullptr) {
            int count = 0;
            for (int i = 0; i < HASH_SIZE; i++) {
                if (hash[i].isValid()) count++;
            }
        } else {
            cout << "Таблица " << tableName << " НЕ загружена!" << endl;
        }
    }
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
int getTuplesLimit() {
    if (schemaConfig.contains("tuples_limit")) {
        return schemaConfig["tuples_limit"];
    }
    return 1000;
}
bool checkForeignKeyExists(const string& tableName, const string& columnName,const string& value) {
    DOUBLE_HASH* tableHash = GET_SCHEMA(tableName);
    if (tableHash == nullptr) {
        cerr << "Хэш таблица '" << tableName << "' не найдена" << endl;
        return false;
    }
    if (!schemaConfig["structure"].contains(tableName)) {
        cerr << "Таблица '" << tableName << "' не найдена в схеме" << endl;
        return false;
    }
    auto columns = schemaConfig["structure"][tableName];
    bool isPrimaryKey = false;
    string pkColumnName = "";
    if (schemaConfig.contains("primary_keys") && 
        schemaConfig["primary_keys"].contains(tableName)) {
        pkColumnName = schemaConfig["primary_keys"][tableName];
        isPrimaryKey = (pkColumnName == columnName);
    }
    for (int i = 0; i < HASH_SIZE; i++) {
        if (tableHash[i].isValid()) {
            Vec rowValues;
            string rowData = tableHash[i].getValue()[0];
            stringstream ss(rowData);
            string cell;
            while (getline(ss, cell, ',')) {
                rowValues.PUSH(cell);
            }
            if (isPrimaryKey) {
                if (!rowValues.empty() && rowValues[0] == value) {
                    return true;
                }
            } else {
                int columnIndex = getColumnIndex(tableName, columnName);
                if (columnIndex != -1 && columnIndex < rowValues.getSize()) {
                    string foundValue = rowValues[columnIndex];
                    if (foundValue == value) {
                        return true;
                    } 
                }
            }
        }
    }
    return false;
}
bool checkForeignKeyForDelete(const string& tableName, const string& columnName, const string& value) {
    if (!schemaConfig.contains("foreign_keys")) {
        return true;
    }
    auto foreignKeys = schemaConfig["foreign_keys"];
    for (auto it = foreignKeys.begin(); it != foreignKeys.end(); ++it) {
        string fkTable = it.key();
        if (fkTable == tableName) continue;
        for (const auto& fk : it.value()) {
            string refTable = fk["references"]["table"];
            string refColumn = fk["references"]["column"];
            if (refTable == tableName && refColumn == columnName) {
                if (checkForeignKeyExists(fkTable, fk["column"], value)) {
                    return false;
                }
            }
        }
    }
    return true;
}
string insertIntoCSV(const string& tableName, Vec& values) {
    const int MAX_RETRIES = 5;
    int retryCount = 0;
    static atomic<int> operationCounter{0};
    int opId = ++operationCounter;
    while (retryCount < MAX_RETRIES) {
        try {
            string tablePath = currentSchemaName + "/" + tableName;
            if (!fs::exists(tablePath)) {
                cerr << "Директория таблицы не существует: " << tablePath << endl;
                return "-1";
            }
            bool needsPrimaryKey = true;
            if (tableName == "user_lot") {
                needsPrimaryKey = false;
            } 
            if (!waitForTableUnlock(tableName, 5000)) {
                cerr << "Таблица '" << tableName << "' заблокирована" << endl;
                return "-1";
            }
            if (!lockTable(tableName)) {
                cerr << "Не удалось заблокировать таблицу '" << tableName << "'" << endl;
                return "-1";
            }
            if (!checkForeignKeysForInsert(tableName, values)) {
                unlockTable(tableName);
                cerr << "Проверка внешних ключей не пройдена" << endl;
                return "-1";
            }
            string primaryKey = "";
            int primaryKeyInt = -1;
            if (needsPrimaryKey) {
                primaryKeyInt = getNextPrimaryKey(tableName);
                primaryKey = to_string(primaryKeyInt);
            } else {
                if (values.getSize() >= 2) {
                    try {
                        int userId = stoi(values[0]);
                        int lotId = stoi(values[1]);
                        primaryKeyInt = userId * 1000 + lotId;
                        primaryKey = to_string(primaryKeyInt);
                    } catch (...) {
                        string composite = values[0] + ":" + values[1];
                        primaryKeyInt = hash<string>{}(composite) % HASH_SIZE;
                        if (primaryKeyInt < 0) primaryKeyInt = -primaryKeyInt;
                        primaryKey = to_string(primaryKeyInt);
                    }
                } else {
                    primaryKeyInt = rand() % 1000000 + 1000;
                    primaryKey = to_string(primaryKeyInt);
                }
            }
            //ВЫБОР CSV ФАЙЛА ДЛЯ ЗАПИСИ
            int tuplesLimit = getTuplesLimit();
            int currentFileNum = 1;
            string currentCsvPath = tablePath + "/1.csv";
            int maxFileNum = 1;
            // Находим файл с максимальным номером
            for (const auto& entry : fs::directory_iterator(tablePath)) {
                if (entry.is_regular_file()) {
                    string filename = entry.path().filename().string();
                    if (hasCsvExtension(filename)) {
                        string numberStr = filename.substr(0, filename.find('.'));
                        try {
                            int fileNum = stoi(numberStr);
                            if (fileNum > maxFileNum) {
                                maxFileNum = fileNum;
                            }
                        } catch (...) {
                            // Игнорируем файлы с нечисловыми именами
                        }
                    }
                }
            }
            currentFileNum = maxFileNum;
            currentCsvPath = tablePath + "/" + to_string(currentFileNum) + ".csv";
            // Проверяем количество строк в файле
            int currentRowCount = getCurrentRowCount(currentCsvPath);
            // Создаем новый файл если нужно
            if (currentRowCount >= tuplesLimit) {
                currentFileNum = maxFileNum + 1;
                currentCsvPath = tablePath + "/" + to_string(currentFileNum) + ".csv";;
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
                    unlockTable(tableName);
                    return "-1";
                }
            }
            //ЗАПИСЬ
            ofstream file(currentCsvPath, ios::app);
            if (!file.is_open()) {
                unlockTable(tableName);
                return "-1";
            }
            // Формируем строку для CSV
            string csvRow;
            if (needsPrimaryKey) {
                csvRow = primaryKey;
                for (size_t i = 0; i < values.getSize(); i++) {
                    csvRow += "," + values[i];
                }
            } else {
                for (size_t i = 0; i < values.getSize(); i++) {
                    if (i > 0) csvRow += ",";
                    csvRow += values[i];
                }
            }
            file << csvRow << endl;
            file.close();
            if (tableName == "order") {
                extern void incrementOrdersWrittenToCSV();
                incrementOrdersWrittenToCSV();
            }
            //РАЗБЛОКИРОВКА ТАБЛИЦЫ
            unlockTable(tableName);
            string hashRow = csvRow;
            DOUBLE_HASH* tableHash = GET_SCHEMA(tableName);
            if (tableHash == nullptr) {
                tableHash = new DOUBLE_HASH[HASH_SIZE];
                if (tableHash == nullptr) {
                    cout << "Не удалось выделить память для хэш-таблицы" << endl;
                    markTableForSync(tableName);
                    return needsPrimaryKey ? primaryKey : "0";
                }
                // Инициализируем все слоты как пустые
                for (int i = 0; i < HASH_SIZE; i++) {
                    tableHash[i].setIsEmpty(true);
                    tableHash[i].setIsDeleted(false);
                }
                if (!ADD_SCHEMA(tableName, tableHash)) {
                    cout << "Не удалось добавить таблицу в SCHEMA_HASH" << endl;
                    delete[] tableHash;
                    markTableForSync(tableName);
                    cout << string(60, '=') << endl;
                    return needsPrimaryKey ? primaryKey : "0";
                }
            }
            if (primaryKeyInt != -1 && tableHash != nullptr) {
                Vec hashValue;
                hashValue.PUSH(hashRow);
                // Вычисляем хэш
                int index = hash1(primaryKeyInt);
                int step = hash2(primaryKeyInt);
                int startIndex = index;
                int attempts = 0;
                while (attempts < HASH_SIZE / 2) {
                    if (tableHash[index].getIsEmpty() || tableHash[index].getIsDeleted()) {
                        tableHash[index].init(primaryKeyInt, hashValue);
                        if (tableName == "order") {
                            extern void incrementOrdersWrittenToHash();
                            incrementOrdersWrittenToHash();
                        }
                        break;
                    }
                    index = (index + step) % HASH_SIZE;
                    attempts++;
                    if (index == startIndex) {
                        break;
                    }
                }
                if (attempts >= HASH_SIZE / 2) {
                    bool found = false;
                    for (int i = 0; i < HASH_SIZE; i++) {
                        if (tableHash[i].getIsEmpty() || tableHash[i].getIsDeleted()) {
                            tableHash[i].init(primaryKeyInt, hashValue);
                            if (tableName == "order") {
                                extern void incrementOrdersWrittenToHash();
                                incrementOrdersWrittenToHash();
                            }
                            found = true;
                            break;
                        }
                    }
                    if (!found) {
                        int newSize = HASH_SIZE * 2;
                        DOUBLE_HASH* newHash = new DOUBLE_HASH[newSize];
                        for (int i = 0; i < HASH_SIZE; i++) {
                            if (tableHash[i].isValid()) {
                                newHash[i] = tableHash[i];
                            }
                        }
                        // Добавляем новую запись в конец
                        newHash[HASH_SIZE].init(primaryKeyInt, hashValue);
                        delete[] tableHash;
                        tableHash = newHash;
                        // Обновляем SCHEMA_HASH
                        ADD_SCHEMA(tableName, tableHash);
                        if (tableName == "order") {
                            extern void incrementOrdersWrittenToHash();
                            incrementOrdersWrittenToHash();
                        }
                    }
                }
            } else {
                cout << "Не удалось добавить в хэш" << endl;
            }
            markTableForSync(tableName);
            // Проверяем, добавилась ли запись в хэш
            if (tableHash != nullptr && primaryKeyInt != -1) {
                int foundIndex = -1;
                for (int i = 0; i < HASH_SIZE; i++) {
                    if (tableHash[i].isValid() && tableHash[i].getKey() == primaryKeyInt) {
                        foundIndex = i;
                        break;
                    }
                }
                if (foundIndex != -1) {
                    Vec& foundValues = tableHash[foundIndex].getValue();
                    
                } else {
                    //ПОВТОРНАЯ ВСТАВКА
                    Vec emergencyHashValue;
                    emergencyHashValue.PUSH(hashRow);
                    for (int i = HASH_SIZE - 100; i < HASH_SIZE; i++) {
                        if (tableHash[i].getIsEmpty()) {
                            tableHash[i].init(primaryKeyInt, emergencyHashValue);
                            if (tableName == "order") {
                                extern void incrementOrdersWrittenToHash();
                                incrementOrdersWrittenToHash();
                            }
                            break;
                        }
                    }
                }
            }
            return needsPrimaryKey ? primaryKey : "0";
        } catch (const exception& e) {
            retryCount++;
            cout << "Ошибка при вставке (попытка " << retryCount << "): " << e.what() << endl;
            if (retryCount >= MAX_RETRIES) {
                cout << "Не удалось вставить после " << MAX_RETRIES << " попыток" << endl;
                string errorLogPath = currentSchemaName + "/insert_errors.log";
                ofstream errorLog(errorLogPath, ios::app);
                if (errorLog.is_open()) {
                    errorLog << "[" << time(nullptr) << "] Ошибка вставки в таблицу " << tableName << ": " << e.what() << endl;
                    errorLog << "  Данные: ";
                    for (size_t i = 0; i < values.getSize(); i++) {
                        if (i > 0) errorLog << ",";
                        errorLog << values[i];
                    }
                    errorLog << endl;
                    errorLog.close();
                }
                return "-1";
            }
            // Ждем перед повторной попыткой
            this_thread::sleep_for(chrono::milliseconds(50 * retryCount));
        }
    }
    return "-1";
}
int insertIntoCSVWithoutFK(const string& tableName, Vec& values) {
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
    // Отмечаем таблицу для фоновой синхронизации
    markTableForSync(tableName);
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
    if (!checkForeignKeyForDelete(tableName, columnName, value)) {
        unlockTable(tableName);
        return false;
    }
    bool success = false;
    if (!schemaConfig["structure"].contains(tableName)) {
        cerr << "Таблица '" << tableName << "' не найдена в схеме" << endl;
        unlockTable(tableName);
        return false;
    }
    auto columns = schemaConfig["structure"][tableName];
    int targetColumnIndex = getColumnIndex(tableName, columnName);
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
    // Отмечаем таблицу для фоновой синхронизации
    if (success) {
        markTableForSync(tableName);
    }
    return success;
}
void cartesianProductFromHash(const Vec& tables, const Vec& columns, const string& whereCond, Vec& result) {
    if (tables.empty()) return;
    extern void processCombination(size_t tableIndex, Vec& currentCombination, const Vec& tables, const Vec& columns, const string& whereCond, Vec& result);
    Vec currentCombination;
    processCombination(0, currentCombination, tables, columns, whereCond, result);
}
bool checkForeignKeysForInsert(const string& tableName, const Vec& values) {
    if (!schemaConfig.contains("foreign_keys") || 
        !schemaConfig["foreign_keys"].contains(tableName)) {
        return true;
    }
    auto foreignKeys = schemaConfig["foreign_keys"][tableName];
    for (const auto& fk : foreignKeys) {
        string column = fk["column"];
        string refTable = fk["references"]["table"];
        string refColumn = fk["references"]["column"];
        int colIndex = getColumnIndex(tableName, column);
        if (colIndex == -1) {
            cerr << "Ошибка: колонка '" << column << "' не найдена в таблице '" << tableName << "'" << endl;
            return false;
        }
        int valueIndex;
        if (tableName == "user_lot") {
            valueIndex = colIndex;
        } else if (tableName == "order") {
            valueIndex = colIndex - 1;
        } else {
            valueIndex = colIndex - 1; 
        }
        if (valueIndex < 0 || valueIndex >= values.getSize()) {
            cerr << "Индекс значения " << valueIndex << " вне диапазона values." << endl;
            return false;
        }
        string fkValue = values[valueIndex];
        if (!fkValue.empty() && !checkForeignKeyExists(refTable, refColumn, fkValue)) {
            cerr << "Значение '" << fkValue << "' не найдено в " << refTable << "." << refColumn << endl;
            return false;
        }
    }
    return true;
}
