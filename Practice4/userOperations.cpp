#include "userOperations.h"
using json = nlohmann::json;
extern string currentSchemaName;
extern json schemaConfig;
string generateUserKey() {// Генерация случайного ключа 
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, 255);
    stringstream ss;
    for (int i = 0; i < 16; i++) {
        int random = dis(gen);
        ss << hex << setw(2) << setfill('0') << random;
    }
    return ss.str();
}
string getUserIdByUsername(const string& username) {
    DOUBLE_HASH* userHash = GET_SCHEMA("user");
    if (userHash == nullptr) {
        cerr << "Хэш таблица 'user' не найдена" << endl;
        return "";
    }
    for (int i = 0; i < HASH_SIZE; i++) {
        if (userHash[i].isValid()) {
            Vec rowValues;
            string rowData = userHash[i].getValue()[0];
            stringstream ss(rowData);
            string cell;
            while (getline(ss, cell, ',')) {
                rowValues.PUSH(cell);
            }
            if (rowValues.getSize() >= 3) {
                string currentUsername = rowValues[1];  
                if (currentUsername == username) {
                    return rowValues[0]; 
                }
            }
            if (rowValues.getSize() >= 4) {
                string currentUsername = rowValues[2];  
                if (currentUsername == username) {
                    return rowValues[0];  
                }
            }
        }
    }
    return "";
}
string getUserIdByKey(const string& userKey) {
    DOUBLE_HASH* userHash = GET_SCHEMA("user");
    if (userHash == nullptr) {
        cerr << "Хэш таблица 'user' не найдена" << endl;
        return "";
    }
    int totalUsers = 0;
    int validEntries = 0;
    for (int i = 0; i < HASH_SIZE; i++) {
        if (userHash[i].isValid()) {
            validEntries++;
            Vec rowValues;
            string rowData = userHash[i].getValue()[0];
            stringstream ss(rowData);
            string cell;
            while (getline(ss, cell, ',')) {
                rowValues.PUSH(cell);
            }
            if (rowValues.getSize() == 3) {
                string id = rowValues[0];      
                string username = rowValues[1]; 
                string key = rowValues[2];      
                if (key == userKey) {
                    return id;
                }
            } else if (rowValues.getSize() > 3) {
                for (size_t j = 0; j < rowValues.getSize(); j++) {
                    if (rowValues[j] == userKey) {
                        string id = rowValues[0];
                        return id;
                    }
                }
            }
            
            totalUsers++;
        }
    }
    return "";
}
bool initializeUserBalance(const string& userId) {
    DOUBLE_HASH* lotHash = GET_SCHEMA("lot");
    if (lotHash == nullptr) {
        cerr << "Таблица 'lot' не найдена" << endl;
        return false;
    }
    bool success = true;
    int lotsAdded = 0;
    for (int i = 0; i < HASH_SIZE; i++) {
        if (lotHash[i].isValid()) {
            Vec lotRowValues;
            string lotRowData = lotHash[i].getValue()[0];
            stringstream ss(lotRowData);
            string cell;
            while (getline(ss, cell, ',')) {
                lotRowValues.PUSH(cell);
            }
            if (lotRowValues.getSize() >= 2) {
                string lotId = lotRowValues[0];
                string lotName = lotRowValues[1];
                // Добавляем 1000 единиц каждого лота пользователю
                Vec balanceValues;
                balanceValues.PUSH(userId);      
                balanceValues.PUSH(lotId);       
                balanceValues.PUSH("1000.0");    
                string result = insertIntoCSV("user_lot", balanceValues);
                if (result == "-1" || result.empty()) {
                    cerr << "Ошибка при добавлении лота " << lotId << " пользователю " << userId << endl;
                    success = false;
                } else {
                    lotsAdded++;
                }
            }
        }
    }
    return success;
}
