#ifndef HTTPCONF_H
#define HTTPCONF_H

#include <string>
#include "json.hpp"
#include "balanceOperations.h"
#include "MYCSV.h"
#include "userOperations.h"
#include <iostream>
#include <sstream>
#include <thread>
#include <chrono>
#include <atomic>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <vector>
#include <algorithm>
#include <ctime>
#include <cstring>
#include <map>
#include <fstream>
using namespace std;

struct PairInfo;
PairInfo getPairInfo(int pair_id);
// Функции для работы с HTTP API
string get_pairs_json();
string get_lots_json();
string create_user_handler(const nlohmann::json& request_data);
string create_order_handler(const std::string& userKey, const nlohmann::json& request_data);
string get_orders_handler();
string delete_order_handler(const std::string& userKey, const nlohmann::json& request_data);
string get_balance_handler(const std::string& userKey);
// Разбор HTTP-запроса
string parse_http_request(const std::string& request);
// Формирование HTTP-ответов
string create_http_response(const std::string& content);
string create_http_error_response(int status_code, const std::string& message);
// Обработка HTTP-клиента
void handle_http_client(int client_socket);
// Запуск HTTP-сервера
void start_http_server(int port);
// Остановка HTTP-сервера
void stop_http_server();
#endif
