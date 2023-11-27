#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>

#define PORT 8888
#define BUFFER_SIZE 1024

void receive_file(SOCKET client_socket) {
    FILE *fp;
    int n;
    char data[BUFFER_SIZE];

    fp = fopen("received_example_file.txt", "wb");
    if (fp == NULL) {
        printf("File opening failed\n");
        return;
    }

    while ((n = recv(client_socket, data, BUFFER_SIZE, 0)) > 0) {
        fwrite(data, 1, n, fp);
    }

    printf("File received\n");

    fclose(fp);
}

int main() {
    WSADATA wsa;
    SOCKET client_socket;
    struct sockaddr_in server_addr;

    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("WSAStartup failed\n");
        return 1;
    }

    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == INVALID_SOCKET) {
        printf("Socket creation failed\n");
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Change this to the server's IP address

    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Connection failed\n");
        return 1;
    }

    receive_file(client_socket);

    closesocket(client_socket);
    WSACleanup();
    return 0;
}
