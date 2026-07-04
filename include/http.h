#ifndef HTTP_H
#define HTTP_H

#include <stddef.h>
#include <sys/types.h>

int send_header(int client_fd, const char *status, const char *contentType,
                size_t contentLength);

int send_text_response(int client_fd, const char *status,
                       const char *content_type, const char *body);

const char *send_file(int client_fd, const char *filepath);

ssize_t write_all(int fd, const void *buffer, size_t count);
#endif