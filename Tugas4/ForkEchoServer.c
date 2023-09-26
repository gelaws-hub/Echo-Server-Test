#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main() {
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(3000);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int sockfd;
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Couldn't create socket");
        return 1;
    }
    printf("Socket created\n");

    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Couldn't bind socket");
        return 1;
    }
    printf("Bound to port 3000\n");

    if (listen(sockfd, 5) < 0) {
        perror("Couldn't listen to socket");
        return 1;
    }
    printf("Listening for connections (max backlog: 5)\n");

    struct sockaddr_in client_addr;
    socklen_t client_addr_size = sizeof(client_addr);

    while (1) {
        int client_sockfd;
        if ((client_sockfd = accept(sockfd, (struct sockaddr *)&client_addr, &client_addr_size)) < 0) {
            perror("Couldn't accept connection");
            return 1;
        }
        printf("Accepted connection from %s\n", inet_ntoa(client_addr.sin_addr));

        pid_t pid = fork(); // Membuat proses anak
        if (pid == 0) { // Kode proses anak
            close(sockfd); // Tutup socket di proses anak
            char msg[100];

            while (1) {
                ssize_t bytes_received = recv(client_sockfd, msg, sizeof(msg), 0);
                if (bytes_received <= 0) {
                    printf("Connection closed or error occurred\n");
                    break;
                }
                msg[bytes_received] = '\0'; // Tambahkan null-terminator

                printf("Received message: %s\n", msg);

                ssize_t bytes_sent = send(client_sockfd, msg, bytes_received, 0);
                if (bytes_sent < 0) {
                    perror("Error sending message");
                    break;
                }

                if (strcmp(msg, "bye") == 0) {
                    printf("Exiting...\n");
                    break;
                }
            }

            close(client_sockfd);
            exit(0); // Keluar dari proses anak
        } else if (pid < 0) {
            perror("Couldn't create child process");
            return 1;
        }

        close(client_sockfd); // Tutup socket di proses utama (parent)
    }

    close(sockfd);
    return 0;
}
