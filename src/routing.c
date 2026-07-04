#include "api.h"
#include "http.h"
#include "logging.h"

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void handle_path(int client_fd, const char *method, const char *path)
{
    if (strcmp(method, "GET") != 0)
    {
        if (send_text_response(client_fd, "405 Method Not Allowed", "text/html",
                               "<h1>405 Method Not Allowed</h1>") == -1)
        {
            perror("send_text_response");
        }

        log_request(method, path, "405 Method Not Allowed");
        return;
    }

    if (strcmp(path, "/api/time") == 0)
    {
        send_time_api(client_fd, path);
        return;
    }
    if (strcmp(path, "/api/status") == 0)
    {
        send_status_api(client_fd, path);
        return;
    }

    if (strcmp(path, "/api/info") == 0)
    {
        send_info_api(client_fd, path);
        return;
    }

    if (strstr(path, "..") != NULL)
    {
        if (send_text_response(client_fd, "403 Forbidden", "text/html",
                               "<h1>403 Forbidden</h1>") == -1)
        {
            perror("send_text_response");
        }
        log_request(method, path, "403 Forbidden");
        return;
    }

    char filepath[256];
    int written = 0;

    if (strcmp(path, "/") == 0)
    {
        written = snprintf(filepath, sizeof(filepath), "public/index.html");
    }
    else if (strrchr(path, '.') == NULL)
    {
        written = snprintf(filepath, sizeof(filepath), "public%s.html", path);
    }
    else
    {
        written = snprintf(filepath, sizeof(filepath), "public%s", path);
    }

    if (written < 0 || (size_t)written >= sizeof(filepath))
    {
        if (send_text_response(client_fd, "414 URI Too Long", "text/html",
                               "<h1> URI Too Long</h1>") == -1)
        {
            perror("send_text_response");
        }

        log_request(method, path, "414 URI Too Long");
        return;
    }

    const char *status = send_file(client_fd, filepath);
    log_request(method, path, status);
}