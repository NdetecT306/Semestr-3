#include "commands.h"
#include "struct.h"
#include "CSV.h"
#include "Block.h"
#include "struct.h"
#include "workWithJson.h"
/*
int main() {
    setlocale(LC_ALL, "rus");
    if (!loadSchema()) {
        schemaConfig["name"] = "Схема 1";
        schemaConfig["tuples_limit"] = 1000;
        schemaConfig["structure"] = json::object();
        currentSchemaName = "Схема 1";
        tuplesLimit = 1000;
        saveSchema();
    }
    if (!createDirectory()) {
        cerr << "Ошибка при создании директории." << endl;
        return 1;
    }
    CREATETABLE(); 
    string command;
    cout << "Доступные команды: INSERT INTO, DELETE FROM, SELECT, EXIT" << endl;
    while (true) {
        cout << "> ";
        getline(cin, command);
        if (command.find("INSERT INTO") == 0) {
            InsertCommand(command);
        }
        else if (command.find("DELETE FROM") == 0) {
            DeleteCommand(command);
        }
        else if (command.find("SELECT") == 0) {
            SelectCommand(command);
        }
        else if (command == "EXIT") {
            break;
        }
        else if (!command.empty()) {
            cout << "Неизвестная команда. Доступные команды: INSERT INTO, DELETE FROM, SELECT, EXIT" << endl;
        }
    }
    CLEAR(); 
    return 0;
}
*/
int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "rus");
    if (argc != 3) {
        cerr << "Использование: " << argv[0] << " --query 'SQL_запрос'" << endl;
        cerr << "Примеры:" << endl;
        cerr << "  " << argv[0] << " --query \"SELECT таблица1.колонка1, таблица2.колонка1 FROM таблица1, таблица2 WHERE таблица1.колонка1 = таблица2.колонка2\"" << endl;
        cerr << "  " << argv[0] << " --query \"INSERT INTO таблица1 VALUES ('somedata', '12345')\"" << endl;
        cerr << "  " << argv[0] << " --query \"DELETE FROM таблица1 WHERE таблица1.колонка1 = '123'\"" << endl;
        return 1;
    }
    string option = argv[1];
    string query = argv[2];
    if (option != "--query") {
        cerr << "Неизвестная опция: " << option << endl;
        cerr << "Использование: " << argv[0] << " --query 'SQL_запрос'" << endl;
        return 1;
    }
    if (!loadSchema()) {// Если схема не загрузилась
        schemaConfig["name"] = "Схема 1";
        schemaConfig["tuples_limit"] = 1000;
        schemaConfig["structure"] = json::object();
        currentSchemaName = "Схема 1";
        tuplesLimit = 1000;
        saveSchema();
    }
    if (!createDirectory()) {
        cerr << "Ошибка при создании директории." << endl;
        return 1;
    }
    CREATETABLE();
    if (query.find("SELECT") == 0) {
        SelectCommand(query);
    }
    else if (query.find("INSERT INTO") == 0) {
        InsertCommand(query);
    }
    else if (query.find("DELETE FROM") == 0) {
        DeleteCommand(query);
    }
    else {
        cerr << "Неизвестный тип запроса. Поддерживаются: SELECT, INSERT INTO, DELETE FROM" << endl;
        cerr << "Полученный запрос: " << query << endl;
        CLEAR();
        return 1;
    }
    CLEAR();
    return 0;
}
