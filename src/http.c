#include "http.h"

#include "mime.h"
#include <fcntl.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

int send_text_response(int client_fd, const char *status,
                       const char *content_type, const char *body)
{
    size_t length = strlen(body);

    if (send_header(client_fd, status, content_type, length) == -1)
    {
        return -1;
    }

    if (write_all(client_fd, body, length) == -1)
    {
        return -1;
    }
    return 0;
}
ssize_t write_all(int fd, const void *buffer, size_t count)
{
    const char *ptr = buffer;
    size_t total = 0;

    while (total < count)
    {
        ssize_t written = write(fd, ptr + total, count - total);

        if (written == -1)
        {
            return -1;
        }

        total += written;
    }

    return total;
}

int send_header(int client_fd, const char *status, const char *content_type,
                size_t content_length)
{
    char header[512];

    int length = snprintf(header, sizeof(header),
                          "HTTP/1.1 %s\r\n"
                          "Content-Type: %s\r\n"
                          "Content-Length: %zu\r\n"
                          "\r\n",
                          status, content_type, content_length);

    if (length < 0 || length >= (int)sizeof(header))
    {
        return -1;
    }

    if (write_all(client_fd, header, length) == -1)
    {
        return -1;
    }

    return 0;
}

const char *send_file(int client_fd, const char *filepath)
{
    int fd = open(filepath, O_RDONLY);

    if (fd == -1)
    {
        if (send_text_response(client_fd, "404 Not Found", "text/html",
                               "<h1>404 Not Found</h1>") == -1)
        {
            perror("send_text_response");
        }
        return "404 Not Found";
    }

    struct stat st;

    if (fstat(fd, &st) == -1)
    {
        perror("fstat");
        close(fd);

        if (send_text_response(client_fd, "500 Internal server error",
                               "text/html",
                               "<h1>500 Internal server error</h1>") == -1)
        {
            perror("send_text_response");
        }
        return "500 Internal server error";
    }

    if (S_ISDIR(st.st_mode))
    {
        close(fd);
        if (send_text_response(client_fd, "403 Forbidden", "text/html",
                               "<h1>403 Forbidden</h1>") == -1)
        {
            perror("send_text_response");
        }
        return "403 Forbidden";
    }

    if (send_header(client_fd, "200 OK", get_content_type(filepath),
                    st.st_size) == -1)
    {
        perror("send_header");
        close(fd);
        return "500 Internal server error";
    }

    char buffer[4096];
    ssize_t n;

    while ((n = read(fd, buffer, sizeof(buffer))) > 0)
    {
        if (write_all(client_fd, buffer, n) == -1)
        {
            perror("write_all");
            close(fd);
            return "500 Internal server error";
        }
    }

    if (n == -1)
    {
        perror("read file");
        close(fd);
        return "500 Internal server error";
    }

    close(fd);
    return "200 OK";
}
