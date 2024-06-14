#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <syslog.h>
#include <errno.h>

#define PORT 9000
#define BUFFER_SIZE 1024
#define FILE_PATH "/var/tmp/aesdsocketdata"

int server_socket = -1;
int client_socket = -1;
FILE *file = NULL;

void handle_signal(int signal) {
    if (signal == SIGINT || signal == SIGTERM) {
        syslog(LOG_INFO, "Caught signal, exiting");
        if (client_socket != -1) {
            close(client_socket);
        }
        if (server_socket != -1) {
            close(server_socket);
        }
        if (file) {
            fclose(file);
            remove(FILE_PATH);
        }
        closelog();
        exit(EXIT_SUCCESS);
    }
}

void setup_signal_handler() {
    struct sigaction sa;
    sa.sa_handler = handle_signal;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGINT, &sa, NULL) == -1 || sigaction(SIGTERM, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }
}

void run_server() {
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, 10) == -1) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    openlog("aesdsocket", LOG_PID, LOG_USER);
    setup_signal_handler();

    while (1) {
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
        if (client_socket == -1) {
            perror("accept");
            continue;
        }
        syslog(LOG_INFO, "Accepted connection from %s", inet_ntoa(client_addr.sin_addr));

        file = fopen(FILE_PATH, "a+");
        if (!file) {
            perror("fopen");
            exit(EXIT_FAILURE);
        }

        ssize_t bytes_received;
        while ((bytes_received = recv(client_socket, buffer, BUFFER_SIZE - 1, 0)) > 0) {
            buffer[bytes_received] = '\0';
            fprintf(file, "%s", buffer);
            fflush(file);
            fseek(file, 0, SEEK_SET);

            char file_buffer[BUFFER_SIZE];
            while (fgets(file_buffer, BUFFER_SIZE, file)) {
                send(client_socket, file_buffer, strlen(file_buffer), 0);
            }
        }

        syslog(LOG_INFO, "Closed connection from %s", inet_ntoa(client_addr.sin_addr));
        fclose(file);
        close(client_socket);
        client_socket = -1;
    }
}

int main(int argc, char *argv[]) {
    if (argc == 2 && strcmp(argv[1], "-d") == 0) {
        pid_t pid = fork();
        if (pid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (pid > 0) {
            exit(EXIT_SUCCESS);
        }
        if (setsid() == -1) {
            perror("setsid");
            exit(EXIT_FAILURE);
        }
        if (chdir("/") == -1) {
            perror("chdir");
            exit(EXIT_FAILURE);
        }
        fclose(stdin);
        fclose(stdout);
        fclose(stderr);
    }

    run_server();

    return 0;
}

