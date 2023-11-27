#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>

#define PORT 8888
#define BUFFER_SIZE 1024

void send_file(FILE *fp, SOCKET client_socket) {
    char data[BUFFER_SIZE];
    int n;

    while ((n = fread(data, 1, sizeof(data), fp)) > 0) {
        if (send(client_socket, data, n, 0) < 0) {
            perror("Error in sending file");
            return;
        }
    }
}

int main() {
    WSADATA wsa;
    SOCKET server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    int addr_len = sizeof(client_addr);
    FILE *fp;
    char *filename = "example_file.txt"; // Change this to the file you want to send

    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("WSAStartup failed\n");
        return 1;
    }

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == INVALID_SOCKET) {
        printf("Socket creation failed\n");
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Binding failed\n");
        return 1;
    }

    if (listen(server_socket, 5) == SOCKET_ERROR) {
        printf("Listening failed\n");
        return 1;
    }

    printf("Server listening...\n");

    while (1) {
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_len);
        if (client_socket == INVALID_SOCKET) {
            printf("Error in accepting connection\n");
            return 1;
        }

        printf("Connection accepted from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        fp = fopen(filename, "rb");
        if (fp == NULL) {
            printf("File opening failed\n");
            return 1;
        }

        send_file(fp, client_socket);
        printf("File sent\n");

        fclose(fp);
        closesocket(client_socket);
    }

    closesocket(server_socket);
    WSACleanup();
    return 0;
}
