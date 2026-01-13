#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <vector>
#include <algorithm>
#include <thread>
#include <chrono>
#include <mutex>
#include <atomic>
#include "json.hpp"
#include "struct.h"
#include "command.h"
#include "lock.h"
#include "MYCSV.h"
#include "connection.h"
#include "HTTPConf.h"
using namespace std;
using json = nlohmann::json;
string currentSchemaName;
json schemaConfig;
string mySchema = "schema.json";
string configFile = "config.json";
mutex hashMutex;
atomic<time_t> lastHashSyncTime{0};

// Функция для загрузки конфигурации
json loadConfig() {
    json config;
    ifstream file(configFile);
    if (file.is_open()) {
        try {
            file >> config;
            cout << "Конфигурация загружена из " << configFile << endl;
        } catch (const exception& e) {
            cerr << "Ошибка загрузки конфигурации: " << e.what() << endl;
        }
        file.close();
    } else {
        config["lots"] = {"RUB", "BTC", "ETH", "USDT", "USDC", "SOL", "DOGE"};
        config["database_ip"] = "localhost";
        config["database_port"] = 7432;
        config["http_port"] = 8080;
        ofstream outfile(configFile);
        if (outfile.is_open()) {
            outfile << config.dump(4);
            outfile.close();
            cout << "Создана конфигурация по умолчанию в " << configFile << endl;
        }
    }
    return config;
}
// Проверка существования лота в БД 
bool lotExists(const string& lotName) {
    DOUBLE_HASH* lotHash = GET_SCHEMA("lot");
    if (lotHash == nullptr) return false;
    for (int i = 0; i < HASH_SIZE; i++) {
        if (lotHash[i].isValid()) {
            Vec rowValues;
            string rowData = lotHash[i].getValue()[0];
            stringstream ss(rowData);
            string cell;
            while (getline(ss, cell, ',')) {
                rowValues.PUSH(cell);
            }
            if (rowValues.getSize() > 1 && rowValues[1] == lotName) {
                return true;
            }
        }
    }
    return false;
}
// Получения лота по имени
string getLotId(const string& lotName) {
    DOUBLE_HASH* lotHash = GET_SCHEMA("lot");
    if (lotHash == nullptr) return "";
    for (int i = 0; i < HASH_SIZE; i++) {
        if (lotHash[i].isValid()) {
            Vec rowValues;
            string rowData = lotHash[i].getValue()[0];
            stringstream ss(rowData);
            string cell;
            while (getline(ss, cell, ',')) {
                rowValues.PUSH(cell);
            }
            if (rowValues.getSize() > 1 && rowValues[1] == lotName) {
                return rowValues[0]; 
            }
        }
    }
    return "";
}
// Для добавления лота в БД 
bool addLot(const string& lotName) {
    Vec values;
    values.PUSH(lotName);  
    string resultStr = insertIntoCSV("lot", values);
    if (resultStr != "-1" && !resultStr.empty()) {
        try {
            int result = stoi(resultStr);
            DOUBLE_HASH* tableHash = GET_SCHEMA("lot");
            if (tableHash != nullptr) {
                string rowData = resultStr + "," + lotName;
                Vec hashValue;
                hashValue.PUSH(rowData);
                int ind = hash1(result);
                int step = hash2(result);
                int startInd = ind;
                int trials = 0;
                while (trials < HASH_SIZE) {
                    if (tableHash[ind].getIsEmpty() || tableHash[ind].getIsDeleted()) {
                        tableHash[ind].init(result, hashValue);
                        break;
                    }
                    ind = (ind + step) % HASH_SIZE;
                    trials++;
                    if (ind == startInd) break;
                }
            }
            return true;
        } catch (const exception& e) {
            cerr << "Ошибка преобразования ID лота: " << e.what() << endl;
            return false;
        }
    }
    cerr << "Ошибка при добавлении лота '" << lotName << "'" << endl;
    return false;
}
// Проверки существования валютной пары
bool pairExists(const string& firstLotId, const string& secondLotId) {
    DOUBLE_HASH* pairHash = GET_SCHEMA("pair");
    if (pairHash == nullptr) return false;
    for (int i = 0; i < HASH_SIZE; i++) {
        if (pairHash[i].isValid()) {
            Vec rowValues;
            string rowData = pairHash[i].getValue()[0];
            stringstream ss(rowData);
            string cell;
            while (getline(ss, cell, ',')) {
                rowValues.PUSH(cell);
            }
            if (rowValues.getSize() > 2) {
                if (rowValues[1] == firstLotId && rowValues[2] == secondLotId) {
                    return true;
                }
            }
        }
    }
    return false;
}
// Добавление валютной пары 
bool addPair(const string& firstLotId, const string& secondLotId) {
    Vec values;
    values.PUSH(firstLotId);
    values.PUSH(secondLotId);
    bool hasForeignKeys = schemaConfig.contains("foreign_keys") && schemaConfig["foreign_keys"].contains("pair");
    json oldForeignKeys;
    if (hasForeignKeys) {
        oldForeignKeys = schemaConfig["foreign_keys"]["pair"];
        schemaConfig["foreign_keys"].erase("pair");
    }
    string resultStr = insertIntoCSV("pair", values);
    if (hasForeignKeys) {
        schemaConfig["foreign_keys"]["pair"] = oldForeignKeys;
    }
    if (resultStr != "-1" && !resultStr.empty()) {
        try {
            int result = stoi(resultStr);
            DOUBLE_HASH* tableHash = GET_SCHEMA("pair");
            if (tableHash != nullptr) {
                string rowData = resultStr + "," + firstLotId + "," + secondLotId;
                Vec hashValue;
                hashValue.PUSH(rowData);
                int ind = hash1(result);
                int step = hash2(result);
                int startInd = ind;
                int trials = 0;
                while (trials < HASH_SIZE) {
                    if (tableHash[ind].getIsEmpty() || tableHash[ind].getIsDeleted()) {
                        tableHash[ind].init(result, hashValue);
                        break;
                    }
                    ind = (ind + step) % HASH_SIZE;
                    trials++;
                    if (ind == startInd) break;
                }
            }
            return true;
        } catch (const exception& e) {
            cerr << "Ошибка преобразования ID пары: " << e.what() << endl;
            return false;
        }
    }
    cerr << "Ошибка при добавлении пары " << firstLotId << "/" << secondLotId << endl;
    return false;
}
// Основная функция инициализации биржи
bool initializeExchange() {
    cout << "КРИПТОБИРЖА \"Сны ИБ-шника о стипендии\"" << endl;
    json config = loadConfig();
    if (!config.contains("lots") || !config["lots"].is_array()) {
        cerr << "В конфигурации отсутствует список лотов" << endl;
        return false;
    }
    vector<string> lots = config["lots"].get<vector<string>>();
    if (lots.empty()) {
        cerr << "Cписок лотов пуст" << endl;
        return false;
    }
    vector<pair<string, string>> lotIds;
    for (const string& lotName : lots) {
        if (addLot(lotName)) {
            this_thread::sleep_for(chrono::milliseconds(50));
            string id = getLotId(lotName);
            if (!id.empty()) {
                lotIds.push_back({lotName, id});
            } else {
                cout << "Не получен ID" << endl;
            }
        } else {
            cout << "ОШИБКА" << endl;
        }
    }
    if (lotIds.empty()) {
        cerr << "Не удалось получить ID ни одного лота" << endl;
        return false;
    }
    loadAllDataToHash();
    int pairsAdded = 0;
    for (size_t i = 0; i < lotIds.size(); i++) {
        for (size_t j = 0; j < lotIds.size(); j++) {
            if (i == j || j < i) continue;
            string firstId = lotIds[i].second;
            string secondId = lotIds[j].second;
            if (!pairExists(firstId, secondId)) {
                if (addPair(firstId, secondId)) {
                    pairsAdded++;
                    this_thread::sleep_for(chrono::milliseconds(20));
                } else {
                    cout << "ОШИБКА" << endl;
                }
            } else {
                cout << "УЖЕ СУЩЕСТВУЕТ" << endl;
            }
        }
    }
    loadAllDataToHash();
    vector<string> allTables = {"user", "order", "user_lot", "lot", "pair"};
    for (const auto& table : allTables) {
        DOUBLE_HASH* hash = GET_SCHEMA(table);
        if (hash != nullptr) {
            int count = 0;
            for (int i = 0; i < HASH_SIZE; i++) {
                if (hash[i].isValid()) count++;
            }
        }
    }
    return true;
}
bool loadSchema() {
    ifstream file(mySchema);
    if (!file.is_open()) {
        cerr << "Не удалось открыть файл " << mySchema << endl;
        return false;
    }
    try {
        file >> schemaConfig;
        currentSchemaName = schemaConfig["name"];
        cout << "Схема '" << currentSchemaName << "' успешно загружена" << endl;
        return true;
    } catch (const exception& e) {
        cerr << "Ошибка загрузки схемы: " << e.what() << endl;
        return false;
    }
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
// Отправка запроса к серверу БД (только для клиентского режима)
string sendQueryToDBServer(const string& query) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        return "Ошибка создания сокета\n";
    }
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(7432);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        close(sock);
        return "Не удалось подключиться к серверу. Убедитесь, что сервер запущен (./database --server)\n";
    }
    ssize_t bytes_sent = send(sock, query.c_str(), query.length(), 0);
    if (bytes_sent <= 0) {
        close(sock);
        return "Ошибка отправки запроса\n";
    }
    char buffer[4096];
    string response;
    ssize_t bytes_received;
    shutdown(sock, SHUT_WR);
    while ((bytes_received = recv(sock, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[bytes_received] = '\0';
        response += buffer;
    }
    if (bytes_received < 0) {
        response += "\nОшибка получения ответа от сервера\n";
    }
    close(sock);
    return response;
}
int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "rus");
    string firstArg = argv[1];
    // Полный сервер БД + HTTP API (Наша криптобиржечка)
    if (firstArg == "--server" || firstArg == "-s") {
        int http_port = 8080;
        bool custom_http_port = false;
        // Проверяем аргументы для порта HTTP
        for (int i = 2; i < argc; i++) {
            string arg = argv[i];
            if (arg == "--http-port" && i + 1 < argc) {
                try {
                    http_port = stoi(argv[i + 1]);
                    custom_http_port = true;
                    i++; // Пропускаем значение порта
                } catch (const exception& e) {
                    cerr << "Ошибка: неверный номер порта HTTP" << endl;
                    return 1;
                }
            }
        }
        // Загружаем схему
        if (!loadSchema()) {
            // Если схема не существует, создаем новую (ОЧЕНЬ много строк)
            schemaConfig["name"] = "Схема 1";
            schemaConfig["tuples_limit"] = 20;
            schemaConfig["structure"] = json::object();
            schemaConfig["primary_keys"] = json::object();
            schemaConfig["foreign_keys"] = json::object();
            currentSchemaName = "Схема 1";
            // Инициализируем структуру таблиц
            json structure = {
                {"user", {"user_id", "username", "key"}},
                {"user_lot", {"user_id", "lot_id", "quantity"}},
                {"order", {"order_id", "user_id", "pair_id", "quantity", "price", "type", "closed"}},
                {"lot", {"lot_id", "name"}},
                {"pair", {"pair_id", "first_lot_id", "second_lot_id"}}
            };
            json primary_keys = {
                {"user", "user_id"},
                {"order", "order_id"},
                {"lot", "lot_id"},
                {"pair", "pair_id"}
            };
            json foreign_keys = {
                {"user_lot", {
                    {{"column", "user_id"}, {"references", {{"table", "user"}, {"column", "user_id"}}}},
                    {{"column", "lot_id"}, {"references", {{"table", "lot"}, {"column", "lot_id"}}}}
                }},
                {"order", {
                    {{"column", "user_id"}, {"references", {{"table", "user"}, {"column", "user_id"}}}},
                    {{"column", "pair_id"}, {"references", {{"table", "pair"}, {"column", "pair_id"}}}}
                }},
                {"pair", {
                    {{"column", "first_lot_id"}, {"references", {{"table", "lot"}, {"column", "lot_id"}}}},
                    {{"column", "second_lot_id"}, {"references", {{"table", "lot"}, {"column", "lot_id"}}}}
                }}
            };
            schemaConfig["structure"] = structure;
            schemaConfig["primary_keys"] = primary_keys;
            schemaConfig["foreign_keys"] = foreign_keys;
            currentSchemaName = "Схема 1";
            // Сохраняем схему в файл
            ofstream outfile(mySchema);
            if (outfile.is_open()) {
                outfile << schemaConfig.dump(4);
                outfile.close();
            }
        }
        // Инициализируем хеш-таблицы
        CREATETABLE();
        CREATE_SCHEMA_TABLE();
        // Создаем директории
        if (!createDirectory()) {
            cerr << "Ошибка при создании директории." << endl;
            return 1;
        }
        // Загружаем существующие данные из CSV в память
        loadAllDataToHash();
        // Инициализируем биржу
        cout << "\nЗапуск биржи" << endl;
        if (!initializeExchange()) {
            cerr << "Ошибка инициализации биржи!" << endl;
            return 1;
        }
        cout << "SQL сервер:" << endl;
        cout << "  Порт: 7432" << endl;
        cout << "  Адрес: localhost (127.0.0.1)" << endl;
        cout << endl;
        cout << "HTTP API сервер:" << endl;
        cout << "  Порт: " << http_port << endl;
        cout << "  Адрес: http://localhost:" << http_port << endl;
        cout << endl;
        cout << "Для остановки сервера нажмите Ctrl+C" << endl;
        // Запускаем HTTP сервер в отдельном потоке
        thread http_thread([http_port]() {
            start_http_server(http_port);
        });
        http_thread.detach();
        // Запускаем SQL сервер (основной поток)
        return main_connection();
    }
    cout << endl;
    return 0;
}
