#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

int main() {
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(3000);
    server_addr.sin_addr.s_addr = inet_addr("192.168.0.102"); // Menggunakan alamat IP server

    int sockfd;
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Couldn't create socket");
        return 1;
    }
    printf("Socket created\n");

    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Couldn't connect");
        return 1;
    }
    printf("Connected to the server\n");

    char msg[100], server_msg[100];
    while (1) {
        printf("Write message : ");
        scanf("%99[^\n]%*c", msg); // Batasi input pesan maksimal menjadi 99 karakter
        send(sockfd, msg, strlen(msg), 0); // Mengirim pesan sesuai panjang pesan yang sebenarnya
        memset(server_msg, 0, sizeof(server_msg));
        ssize_t bytes_received = recv(sockfd, server_msg, sizeof(server_msg), 0);
        if (bytes_received <= 0) {
            perror("Connection closed or error occurred");
            break;
        }
        printf("Server reply : %s\n", server_msg);
        if (strcmp(server_msg, "bye") == 0) {
            printf("Exiting...\n");
            break;
        }
    }

    close(sockfd);
    return 0;
}
