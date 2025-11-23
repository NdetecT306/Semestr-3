#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <random>
#include <filesystem>
#include <thread>
#include <chrono>
#include <functional>
#include "json.hpp"
#include "struct.h"
#include "command.h"
#include "lock.h"
#include "MYCSV.h"
using namespace std;
using json = nlohmann::json;
namespace fs = std::filesystem;
string currentSchemaName = "";
json schemaConfig;
string mySchema = "schema.json";
bool loadSchema() { //Работа с JSON
    ifstream file(mySchema);
    if (!file.is_open()) {
        cerr << "Не удалось открыть файл " << mySchema << endl;
        return false;
    }
    file >> schemaConfig;
    currentSchemaName = schemaConfig["name"];
    cout << "Схема '" << currentSchemaName << "' успешно загружена" << endl;
    return true;
}
bool saveSchema() {
    ofstream file(mySchema);
    if (!file.is_open()) {
        cerr << "Не удалось открыть файл " << mySchema << " для записи" << endl;
        return false;
    }
    try {
        file << schemaConfig.dump(4);
        return true;
    } catch (const exception& e) {
        cerr << e.what() << endl;
        return false;
    }
}
int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "rus");
    
    // Проверка количества аргументов
    if (argc != 2) {
        cerr << "Использование: " << argv[0] << " \"SQL_запрос\"" << endl;
        cerr << "Примеры:" << endl;
        cerr << "  " << argv[0] << " \"SELECT таблица1.колонка1, таблица2.колонка1 FROM таблица1, таблица2 WHERE таблица1.колонка1 = таблица2.колонка2\"" << endl;
        cerr << "  " << argv[0] << " \"INSERT INTO таблица1 VALUES ('somedata', '12345')\"" << endl;
        cerr << "  " << argv[0] << " \"DELETE FROM таблица1 WHERE таблица1.колонка1 = '123'\"" << endl;
        return 1;
    }
    
    string query = argv[1];
    
    // Загрузка или создание схемы
    if (!loadSchema()) {
        schemaConfig["name"] = "Схема 1";
        schemaConfig["tuples_limit"] = 1000;
        schemaConfig["structure"] = json::object();
        currentSchemaName = "Схема 1";
        saveSchema();
    }
    
    // Инициализация системы
    if (!createDirectory()) {
        cerr << "Ошибка при создании директории." << endl;
        return 1;
    }
    CREATETABLE();
    CREATE_SCHEMA_TABLE();
    loadAllDataToHash();
    
    // Обработка SQL запроса
    if (query.find("SELECT") == 0) {
        SelectCommand(query);
    } else if (query.find("INSERT INTO") == 0) {
        InsertCommand(query);
    } else if (query.find("DELETE FROM") == 0) {
        DeleteCommand(query);
    } else {
        cerr << "Неизвестный тип запроса. Поддерживаются: SELECT, INSERT INTO, DELETE FROM" << endl;
        cerr << "Полученный запрос: " << query << endl;
        CLEAR();
        CLEAR_SCHEMA_TABLE();
        return 1;
    }
    
    // Очистка ресурсов
    CLEAR();
    CLEAR_SCHEMA_TABLE();
    return 0;
}
