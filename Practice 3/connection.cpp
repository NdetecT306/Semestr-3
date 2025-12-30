#include "connection.h"
using namespace std;
using json = nlohmann::json;
extern string currentSchemaName;
extern json schemaConfig;
extern string mySchema;
mutex mut;
atomic<bool> serverrun{true};

void handle_client(int clientsock) {
    char buffer[4096];
    memset(buffer, 0, sizeof(buffer));
    ssize_t bytes_received = recv(clientsock, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received <= 0) {
        close(clientsock);
        return;
    }
    string query(buffer, bytes_received);
    size_t start = query.find_first_not_of(" \t\n\r");// Удаляем начальные и конечные пробелы
    size_t end = query.find_last_not_of(" \t\n\r");
    if (start == string::npos || end == string::npos) {
        string response = "ERROR: Пустой запрос\n";
        send(clientsock, response.c_str(), response.length(), 0);
        close(clientsock);
        return;
    }
    query = query.substr(start, end - start + 1);
    struct sockaddr_in client_addr; //IP
    socklen_t addr_len = sizeof(client_addr);
    getpeername(clientsock, (struct sockaddr*)&client_addr, &addr_len);
    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
    cout << "ЗАПРОС ОТ КЛИЕНТА " << client_ip << ":" << endl;
    cout << query << endl;
    string response;
    lock_guard<mutex> lock(mut); //Блокировка
    if (currentSchemaName.empty()) {
        ifstream file(mySchema);
        if (file.is_open()) {
            file >> schemaConfig;
            currentSchemaName = schemaConfig["name"];
            cout << "Схема '" << currentSchemaName << "' загружена" << endl;
        } else {
            // Создаем новую схему при ошибке загрузки (ХОТЯ НЕ ДОЛЖНО БЫТЬ)
            schemaConfig["name"] = "Схема 1";
            schemaConfig["tuples_limit"] = 20;
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
        file.close();
    }
    // Создаем директории для таблиц
    if (!createDirectory()) {
        response = "Не удалось создать директорию данных\n";
        send(clientsock, response.c_str(), response.length(), 0);
        close(clientsock);
        return;
    }
    // Инициализируем хеш-таблицы
    CREATETABLE();
    CREATE_SCHEMA_TABLE();
    loadAllDataToHash();
    // Перенаправляем вывод в строку
    streambuf* old_cout_buf = cout.rdbuf();
    stringstream response_stream;
    cout.rdbuf(response_stream.rdbuf());
    try {
        if (query.find("SELECT") == 0 || query.find("select") == 0) {
            SelectCommand(query);
        } else if (query.find("INSERT INTO") == 0 || query.find("insert into") == 0) {
            InsertCommand(query);
        } else if (query.find("DELETE FROM") == 0 || query.find("delete from") == 0) {
            DeleteCommand(query);
        } else {
            cout << "Неизвестный тип запроса" << endl;
            cout << "Поддерживаемые запросы: SELECT, INSERT INTO, DELETE FROM" << endl;
        }
    } catch (const exception& e) {
        cout << "Ошибка обработки запроса: " << e.what() << endl;
    }
    // Восстанавливаем стандартный вывод
    cout.rdbuf(old_cout_buf);
    response = response_stream.str();
    // Очищаем хеш-таблицы для следующего запроса
    CLEAR();
    CLEAR_SCHEMA_TABLE();
    // Отправляем ответ клиенту
    ssize_t bytes_sent = send(clientsock, response.c_str(), response.length(), 0);
    if (bytes_sent <= 0) {
        cerr << "Ошибка отправки ответа клиенту" << endl;
    }
    close(clientsock);
}
void start_server(int port) {
    int serversock, clientsock;
    struct sockaddr_in serveraddr, clientaddr;
    socklen_t clientlen = sizeof(clientaddr);
    // Создаем сокет
    serversock = socket(AF_INET, SOCK_STREAM, 0);
    if (serversock < 0) {
        cerr << "Ошибка создания сокета сервера" << endl;
        return;
    }
    // Настраиваем адрес сервера
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = INADDR_ANY;
    serveraddr.sin_port = htons(port);
    // Привязываем сокет к адресу
    if (bind(serversock, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) < 0) {
        cerr << "Ошибка привязки сокета к порту " << port << endl;
        close(serversock);
        return;
    }
    // Начинаем прослушивание
    if (listen(serversock, 10) < 0) {
        cerr << "Ошибка начала прослушивания порта" << endl;
        close(serversock);
        return;
    }
    cout << "Сервер запущен и прослушивает порт " << port << endl;
    while (serverrun) {
        // Принимаем входящее соединение
        clientsock = accept(serversock, (struct sockaddr*)&clientaddr, &clientlen);
        if (clientsock < 0) {
            if (serverrun) {
                cerr << "Ошибка принятия соединения" << endl;
            }
            continue;
        }
        // Выводим информацию о подключившемся клиенте
        char clientip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &clientaddr.sin_addr, clientip, INET_ADDRSTRLEN);
        cout << "Клиент " << clientip << ":" << ntohs(clientaddr.sin_port) << endl;
        // Создаем поток для обработки клиента
        thread client_thread(handle_client, clientsock);
        client_thread.detach();
    }
    // Закрываем серверный сокет
    close(serversock);
    cout << "Сервер остановлен" << endl;
}
// Функция для остановки сервера
void stop_server() {
    serverrun = false;
    // Создаем временное соединение, чтобы выйти из accept()
    int temp_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (temp_socket >= 0) {
        struct sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = inet_addr("127.0.0.1");
        addr.sin_port = htons(7432);
        connect(temp_socket, (struct sockaddr*)&addr, sizeof(addr));
        close(temp_socket);
    }
}
// Основная функция для запуска сервера
int main_connection() {
    start_server(7432);
    return 0;
}
