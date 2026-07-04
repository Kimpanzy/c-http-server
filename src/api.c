#include "api.h"
#include "http.h"
#include "logging.h"

#include <stdio.h>
#include <string.h>
#include <time.h>

void send_time_api(int client_fd, const char *path)
{
    time_t now = time(NULL);
    struct tm *time_info = localtime(&now);

    char time_str[64];
    strftime(time_str, sizeof(time_str), "%H:%M:%S", time_info);

    char body[128];
    snprintf(body, sizeof(body), "{\"time\":\"%s\"}", time_str);

    if (send_text_response(client_fd, "200 OK", "application/json", body) == -1)
    {
        perror("send_text_response");
    }
    log_request("GET", path, "200 OK");
}

void send_status_api(int client_fd, const char *path)
{
    const char *body = "{\"status\":\"ok\"}";

    if (send_text_response(client_fd, "200 OK", "application/json", body) == -1)
    {
        perror("send_text_response");
    }
    log_request("GET", path, "200 OK");
}

void send_info_api(int client_fd, const char *path)
{
    const char *body = "{\"server\":\"c-http-server\",\"version\":\"0.1\"}";

    if (send_text_response(client_fd, "200 OK", "application/json", body) == -1)
    {
        perror("send_text_response");
    }
    log_request("GET", path, "200 OK");
}