#include "logging.h"
#include <stdio.h>
#include <time.h>

void log_request(const char *method, const char *path, const char *status)
{

    time_t now = time(NULL);
    struct tm *time_info = localtime(&now);

    char time_str[64];

    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", time_info);

    printf("[%s] %s %s -> %s\n", time_str, method, path, status);
}