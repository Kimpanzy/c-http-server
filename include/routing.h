#ifndef ROUTING_H
#define ROUTING_H
void handle_path(int client_fd, const char *method, const char *path);
void send_time_api(int client_fd);
#endif