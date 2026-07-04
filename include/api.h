#ifndef API_H
#define API_H

void send_time_api(int client_fd, const char *path);
void send_status_api(int client_fd, const char *path);
void send_info_api(int client_fd, const char *path);

#endif