#ifndef SERVER_H
#define SERVER_H

void run_server(void);
int create_server_socket(void);
void handle_client(int client_fd);

#endif