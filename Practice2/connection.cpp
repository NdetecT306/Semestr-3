#include <iostream>
#include <cstring>
#include <string>
#include <thread>
#include <vector>
#include <mutex>
#include <atomic>
#include <fstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "command.h"
#include "lock.h"
#include "MYCSV.h"
#include "json.hpp"
#include "struct.h"
using namespace std;
using json = nlohmann::json;
extern string currentSchemaName;
extern json schemaConfig;
extern string mySchema;
mutex mut;
atomic<bool> serverrun{true};
void handleClient(int clientsock) {
    char buffer[4096];
    memset(buffer, 0, sizeof(buffer));
    ssize_t bytes_received = recv(clientsock, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received <= 0) {
        close(clientsock);
        return;
    }
    string query(buffer, bytes_received);
    size_t start = query.find_first_not_of(" \t\n\r");
    size_t end = query.find_last_not_of(" \t\n\r");
    if (start == string::npos || end == string::npos) {
        string response = "Пустой запрос\n";
        send(clientsock, response.c_str(), response.length(), 0);
        close(clientsock);
        return;
    }
    query = query.substr(start, end - start + 1);
    cout << "Запрос от клиента: " << query << endl;
    string response;
    lock_guard<mutex> lock(mut);
    if (currentSchemaName.empty()) {
        ifstream file(mySchema);
        if (file.is_open()) {
            file >> schemaConfig;
            currentSchemaName = schemaConfig["name"];
            cout << "Schema '" << currentSchemaName << "' loaded" << endl;
        } else {
            schemaConfig["name"] = "Schema 1";
            schemaConfig["tuples_limit"] = 1000;
            schemaConfig["structure"] = json::object();
            currentSchemaName = "Schema 1";
        }
    }
    if (!createDirectory()) {
        response = "Не удалось создать директорию\n";
        send(clientsock, response.c_str(), response.length(), 0);
        close(clientsock);
        return;
    }
    CREATETABLE();
    CREATE_SCHEMA_TABLE();
    loadAllDataToHash();
    streambuf* old_cout_buf = cout.rdbuf();
    stringstream response_stream;
    cout.rdbuf(response_stream.rdbuf());
    try {
        if (query.find("SELECT") == 0) {
            SelectCommand(query);
        } else if (query.find("INSERT INTO") == 0) {
            InsertCommand(query);
        } else if (query.find("DELETE FROM") == 0) {
            DeleteCommand(query);
        } else {
            cout << "Неизвестный запрос" << endl;
        }
    } catch (const exception& e) {
        cout << "Ошибка обработки запроса: " << e.what() << endl;
    }
    cout.rdbuf(old_cout_buf);
    response = response_stream.str();
    CLEAR();
    CLEAR_SCHEMA_TABLE();
    send(clientsock, response.c_str(), response.length(), 0);
    close(clientsock);
    cout << "Сжигаем мосты с клиентом" << endl;
}
void startServer(int port = 7432) {
    int serversock, clientsock;
    struct sockaddr_in serveraddr, clientaddr;
    socklen_t clientlen = sizeof(clientaddr);
    serversock = socket(AF_INET, SOCK_STREAM, 0);
    if (serversock < 0) {
        cerr << "Ошибка создания сокета" << endl;
        return;
    }
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = INADDR_ANY;
    serveraddr.sin_port = htons(port);
    if (bind(serversock, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) < 0) {
        cerr << "Передача сокетов пошла не по порту" << port << endl;
        close(serversock);
        return;
    }
    if (listen(serversock, 10) < 0) {
        cerr << "Мы не слышим сокет" << endl;
        close(serversock);
        return;
    }
    cout << "Сервер начал работу на порту " << port << endl;
    cout << "Ожидаем соединение..." << endl;
    while (serverrun) {
        // Принимаем соединение
        clientsock = accept(serversock, (struct sockaddr*)&clientaddr, &clientlen);
        if (clientsock < 0) {
            if (serverrun) {
                cerr << "Ошибка принятия" << endl;
            }
            continue;
        }
        // Выводим информацию о подключившемся клиенте
        char clientip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &clientaddr.sin_addr, clientip, INET_ADDRSTRLEN);
        cout << "Клиент подсоединен к " << clientip << ":" << ntohs(clientaddr.sin_port) << endl;
        // Создаем поток для обработки клиента
        thread client_thread(handleClient, clientsock);
        client_thread.detach(); 
    }
    // Закрываем серверный сокет
    close(serversock);
    cout << "Сервер остановлен" << endl;
}
// Функция для остановки сервера
void stopServer() {
    serverrun = false;
    int vremsocket = socket(AF_INET, SOCK_STREAM, 0);
    if (vremsocket >= 0) {
        struct sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = inet_addr("172.17.0.1");
        addr.sin_port = htons(7432);
        connect(vremsocket, (struct sockaddr*)&addr, sizeof(addr));
        close(vremsocket);
    }
}
// Основная функция для запуска сервера 
int main_connection() {
    startServer();
    return 0;
}
