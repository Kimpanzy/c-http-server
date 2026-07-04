#define _POSIX_C_SOURCE 200809L

#include "server.h"
#include "http.h"
#include "routing.h"

#include <errno.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>

static volatile sig_atomic_t running = 1;

static void handle_sigint(int sig)
{
    (void)sig;
    running = 0;
}

int create_server_socket(void)
{
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (server_fd == -1)
    {
        perror("socket");
        exit(1);
    }

    int opt = 1;

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) ==
        -1)
    {
        perror("setsockopt");
        exit(1);
    }

    printf("Socket created: fd=%d\n", server_fd);

    struct sockaddr_in server = {0};

    server.sin_family = AF_INET;
    server.sin_port = htons(8080);
    server.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    if (bind(server_fd, (struct sockaddr *)&server, sizeof(server)) == -1)
    {
        perror("bind");
        exit(1);
    }

    if (listen(server_fd, 5) == -1)
    {
        perror("listen");
        exit(1);
    }
    return server_fd;
}

static int parse_request_line(char *buffer, char **method, char **path)
{
    char *line = strtok(buffer, "\r\n");

    if (line == NULL)
    {
        return -1;
    }

    *method = strtok(line, " ");
    *path = strtok(NULL, " ");
    char *version = strtok(NULL, " ");

    if (*method == NULL || *path == NULL || version == NULL)
    {
        return -1;
    }

    return 0;
}

void handle_client(int client_fd)
{
    char buffer[4096];

    ssize_t n = read(client_fd, buffer, sizeof(buffer) - 1);

    if (n == -1)
    {
        perror("read");
    }
    else
    {
        buffer[n] = '\0';

        char *method = NULL;
        char *path = NULL;

        if (parse_request_line(buffer, &method, &path) == -1)
        {
            if (send_text_response(client_fd, "400 Bad Request", "text/html",
                                   "<h1>400 Bad Request</h1>") == -1)
            {
                perror("send_text_response");
            }
            return;
        }

        handle_path(client_fd, method, path);
    }
}
static void handle_sigchld(int sig)
{
    (void)sig;

    int saved_errno = errno;

    while (waitpid(-1, NULL, WNOHANG) > 0)
    {
        // clean children
    }

    errno = saved_errno;
}

static void setup_signal_handlers(void)
{
    struct sigaction sa_chld = {0};
    sa_chld.sa_handler = handle_sigchld;
    sigemptyset(&sa_chld.sa_mask);
    sa_chld.sa_flags = SA_RESTART;

    if (sigaction(SIGCHLD, &sa_chld, NULL) == -1)
    {
        perror("sigaction SIGCHLD");
        exit(1);
    }

    struct sigaction sa_int = {0};
    sa_int.sa_handler = handle_sigint;
    sigemptyset(&sa_int.sa_mask);
    sa_int.sa_flags = 0;

    if (sigaction(SIGINT, &sa_int, NULL) == -1)
    {
        perror("sigaction SIGINT");
        exit(1);
    }
}

void run_server(void)
{
    setup_signal_handlers();

    int server_fd = create_server_socket();

    while (running)
    {
        int client_fd = accept(server_fd, NULL, NULL);

        if (client_fd == -1)
        {
            if (errno == EINTR)
            {
                continue;
            }
            perror("accept");
            continue;
        }

        pid_t pid = fork();

        // child
        if (pid == 0)
        {
            close(server_fd);

            handle_client(client_fd);

            close(client_fd);

            exit(0);
        }
        else if (pid > 0)
        {
            // parent
            close(client_fd);
        }
        else
        {
            perror("fork");
            close(client_fd);
        }
    }
    printf("\nServer shutting down...\n");
    close(server_fd);
}