#ifndef CONNECTION_H
#define CONNECTION_H

void start_server(int port = 7432);
void stop_server();
void handle_client(int client_socket);
int main_connection();

#endif 
