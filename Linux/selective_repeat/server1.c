
#include "headerFiles.h"
#define BUFFER_SIZE 5
#define WINDOW_SIZE 4
struct Packet {
    int sequenceNumber;
    char data[BUFFER_SIZE];
};
int main(void) {
    int listensd, connectsd, n, bytesToRecv, processID;
    char buffer[256];
    char* movePtr;
    struct sockaddr_in serverAddress, clientAddress;
    socklen_t clAddrLen;  // Use socklen_t for accept() parameter

    clAddrLen = sizeof(clientAddress);
    listensd = socket(AF_INET, SOCK_STREAM, 0);
    if (listensd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddress.sin_port = htons(8080);

    if (bind(listensd, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        perror("bind");
        close(listensd);
        exit(EXIT_FAILURE);
    }
    int reuse = 1;
    if (setsockopt(listensd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) == -1) {
        perror("setsockopt");
        close(listensd);
        exit(EXIT_FAILURE);
    }
    if (listen(listensd, 5) == -1) {
        perror("listen");
        close(listensd);
        exit(EXIT_FAILURE);
    }
    printf("Listening...\n");
    for (;;) {
        connectsd = accept(listensd, (struct sockaddr*)&clientAddress, &clAddrLen);
        if (connectsd == -1) {
            perror("accept");
            close(listensd);
            exit(EXIT_FAILURE);
        }
        printf("Connected: %s\n", inet_ntoa(clientAddress.sin_addr));
        processID = fork();

        if (processID == 0) {
            FILE *fp=fopen("output.txt","w");
            if(fp==NULL){
                perror("fopen");
                exit(EXIT_FAILURE); 
            }
            struct Packet receivedPacket;
            memset(&receivedPacket, 0, sizeof(struct Packet));
            n = recv(connectsd, &receivedPacket, sizeof(struct Packet), 0);
            while (n > 0) {
                printf("Received packet with sequence number: %d\n", receivedPacket.sequenceNumber);
                fwrite(receivedPacket.data, sizeof(char), strlen(receivedPacket.data), fp);
                memset(&receivedPacket, 0, sizeof(struct Packet));
                n = recv(connectsd, &receivedPacket, sizeof(struct Packet), 0);
            }
        } else if (processID == -1) {
            perror("fork");
        } else {
            close(connectsd);
        }
    }

    close(listensd);  // Close server socket outside the loop

    return 0;
}
