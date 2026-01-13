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
#include <arpa/inet.h>
#include "MYCSV.h"
#include "command.h"
#include "json.hpp"
#include <sstream>
#include <chrono>
#include <unistd.h>
void start_server(int port = 7432);
void stop_server();
void handle_client(int client_socket);
int main_connection();

