#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "json.hpp"
#include "struct.h"
#include "command.h"
#include "lock.h"
#include "MYCSV.h"
using namespace std;
using json = nlohmann::json;
string currentSchemaName;
json schemaConfig;
string mySchema = "schema.json";

bool loadSchema() {
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
string sendQueryToServer(const string& query, const string& server_ip = "172.17.0.1", int port = 7432) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        return "Ошибка создания сокета";
    }
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, server_ip.c_str(), &server_addr.sin_addr) <= 0) {
        close(sock);
        return "Неправильный ip";
    }
    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        close(sock);
        return "ТЫ НЕ ПРОЙДЕШЬ! (ошибка подключения к серверу)";
    }
    // Отправляем запрос
    send(sock, query.c_str(), query.length(), 0);
    // Получаем ответ
    char buffer[4096];
    string response;
    ssize_t bytes_received;
    while ((bytes_received = recv(sock, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[bytes_received] = '\0';
        response += buffer;
        if (bytes_received < sizeof(buffer) - 1) {
            break;
        }
    }
    close(sock);
    return response;
}

int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "rus");
    // Проверка аргументов командной строки
    if (argc < 2) {
        cerr << "Использование:" << endl;
        cerr << "  Запуск сервера: " << argv[0] << " --server" << endl;
        cerr << "  Запуск клиента: " << argv[0] << " \"SQL_запрос\"" << endl;
        cerr << "  Запуск клиента с указанием сервера: " << argv[0] << " --client \"SQL_запрос\" [IP_адрес]" << endl;
        cerr << endl;
        cerr << "Примеры:" << endl;
        cerr << "  Запуск сервера: " << argv[0] << " --server" << endl;
        cerr << "  Простой запрос: " << argv[0] << " \"SELECT users.id, users.name FROM users\"" << endl;
        cerr << "  Запрос к другому серверу: " << argv[0] << " --client \"INSERT INTO users VALUES ('Ivan', 'ivan@mail.ru', 25)\" 192.168.1.100" << endl;
        return 1;
    }
    // Режим сервера
    if (string(argv[1]) == "--server") {
        // Загружаем схему
        if (!loadSchema()) {
            schemaConfig["name"] = "Схема 1";
            schemaConfig["tuples_limit"] = 1000;
            schemaConfig["structure"] = json::object();
            currentSchemaName = "Схема 1";
            saveSchema();
        }
        // Инициализируем систему
        if (!createDirectory()) {
            cerr << "Ошибка при создании директории." << endl;
            return 1;
        }
        cout << "Запуск сервера базы данных на порту 7432..." << endl;
        cout << "Для остановки сервера нажмите Ctrl+C" << endl;
        extern int main_connection();
        return main_connection();
    }
    // Режим клиента
    string query;
    string server_ip = "127.0.0.1";
    if (string(argv[1]) == "--client") {
        if (argc < 3) {
            cerr << "Ошибка: для режима --client требуется SQL-запрос" << endl;
            return 1;
        }
        query = argv[2];
        if (argc >= 4) {
            server_ip = argv[3];
        }
    } else {
        query = argv[1];
    }
    if (!loadSchema()) {
        schemaConfig["name"] = "Схема 1";
        schemaConfig["tuples_limit"] = 1000;
        schemaConfig["structure"] = json::object();
        currentSchemaName = "Схема 1";
        saveSchema();
    }
    cout << "Отправка запроса на сервер " << server_ip << ":7432..." << endl;
    cout << "Запрос: " << query << endl;
    string response = sendQueryToServer(query, server_ip);
    cout << response;
    if (response.find("ERROR") == 0) {
        cout << endl;
        cout << "Советы:" << endl;
        cout << "1. Убедитесь, что сервер запущен: " << argv[0] << " --server" << endl;
        cout << "2. Проверьте правильность SQL-запроса" << endl;
        cout << "3. Убедитесь, что таблицы существуют в схеме" << endl;
    }
    return 0;
}
