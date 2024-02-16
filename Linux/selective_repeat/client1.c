#include "headerFiles.h"
#include <stdbool.h>

#define BUFFER_SIZE 5
#define WINDOW_SIZE 4
#define TIMEOUT_SEC 5

struct Packet {
    int sequenceNumber;
    char data[BUFFER_SIZE];
};

int main(void) {
    int sd, n;
    struct sockaddr_in serverAddress;
    FILE *fp;
    char sendBuffer[BUFFER_SIZE];
    int base = 0;
    int nextSeqNum = 0;
    struct Packet window[WINDOW_SIZE];
    bool acknowledged[WINDOW_SIZE] = {false};

    sd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    if (inet_pton(AF_INET, "127.0.0.1", &serverAddress.sin_addr) <= 0) {
        perror("inet_pton");
        close(sd);
        exit(EXIT_FAILURE);
    }
    serverAddress.sin_port = htons(8080);

    if (connect(sd, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        perror("connect");
        close(sd);
        exit(EXIT_FAILURE);
    }

    fp = fopen("input.txt", "r");
    if (fp == NULL) {
        perror("fopen");
        close(sd);
        exit(EXIT_FAILURE);
    }

    while (base < WINDOW_SIZE) {
        // Send packets within the window
        for (int i = base; i < nextSeqNum; i++) {
            if (!acknowledged[i]) {
                struct Packet packet;
                packet.sequenceNumber = i;
                memset(packet.data, 0, sizeof(packet.data));
                n = fread(packet.data, sizeof(char), BUFFER_SIZE - 1, fp);

                if (n == 0) {
                    // End of file reached
                    break;
                }
                // Send the packet to the server
                send(sd, &packet, sizeof(struct Packet), 0);
                printf("Sent packet with sequence number: %d\n", packet.sequenceNumber);
            }
        }

        // Receive acknowledgments
        for(int i=0;i<WINDOW_SIZE;i++){
            struct Packet ackPacket;
            n = recv(sd, &ackPacket, sizeof(struct Packet), 0);
            printf("Received acknowledgment for sequence number: %d\n", ackPacket.sequenceNumber);
            acknowledged[ackPacket.sequenceNumber] = true;
        }
    }

    fclose(fp);
    close(sd);

    return 0;
}
