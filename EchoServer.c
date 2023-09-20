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
    // Membuat objek untuk alamat server
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;             // Menggunakan protokol IPv4
    server_addr.sin_port = htons(4002);           // Menentukan port (4002)
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY); // Mengikuti alamat IP lokal

    int sockfd;

    // Membuat socket
    if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        printf("couldn't create socket\n");
        return 1;
    }
    printf("socket created\n");

    // Mengikat socket ke alamat server
    if (bind(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        printf("couldn't bind socket\n");
        return 1;
    }
    printf("bind at port 4002\n");

    // Mendengarkan koneksi dengan batasan antrian 5
    if (listen(sockfd, 5) < 0) {
        printf("couldn't listen to socket\n");
        return 1;
    }
    printf("listening connection: 5\n");

    struct sockaddr_in client_addr;
    int client_addr_size = sizeof(client_addr);

    while (1) {
        int client_sockfd;

        // Menerima koneksi dari client
        if ((client_sockfd = accept(sockfd, (struct sockaddr *) &client_addr, &client_addr_size)) < 0) {
            printf("couldn't accept connection\n");
            return 1;
        }
        printf("accepted connection from %s\n", inet_ntoa(client_addr.sin_addr));

        char msg[100];

        while(1) {
            // Menerima pesan dari client
            recv(client_sockfd, msg, sizeof(msg), 0);
            printf("msg recv = %s\n", msg);

            // Mengirim pesan kembali ke client
            send(client_sockfd, msg, sizeof(msg), 0);

            // Memeriksa apakah pesan adalah "bye" untuk keluar
            if (strcmp(msg, "bye") == 0) {
                printf("exiting...\n");
                break;
            }
        }

        // Menutup koneksi dengan client
        close(client_sockfd);
    }

    // Menutup socket server
    close(sockfd);

    return 0;
}
