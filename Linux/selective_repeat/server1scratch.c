#include "headerFiles.h"
#include <stdbool.h>

#define BUFFER_SIZE 5
#define WINDOW_SIZE 4

struct Packet {
    int sequenceNumber;
    char data[BUFFER_SIZE];
};

int main(void) {
    int sd, new_sd, n;
    struct sockaddr_in serverAddress, clientAddress;
    socklen_t clientLength;
    char recvBuffer[sizeof(struct Packet)];
    int expectedSeqNum = 0;

    sd = socket(AF_INET, SOCK_STREAM, 0);
    // ... (error handling for socket creation)

    // Set up server address
    // ...

    // Bind and listen for incoming connections
    // ...

    clientLength = sizeof(clientAddress);
    new_sd = accept(sd, (struct sockaddr*)&clientAddress, &clientLength);

    if (new_sd == -1) {
        perror("accept");
        close(sd);
        exit(EXIT_FAILURE);
    }

    // Main loop to receive packets and send acknowledgments
    while (true) {
        struct Packet receivedPacket;
        n = recv(new_sd, &receivedPacket, sizeof(struct Packet), 0);

        if (n == 0) {
            // Connection closed by the client
            break;
        } else if (n < 0) {
            perror("recv");
            // Handle error or break the loop
            break;
        }

        printf("Received packet with sequence number: %d\n", receivedPacket.sequenceNumber);

        if (receivedPacket.sequenceNumber == expectedSeqNum) {
            // Packet in order, process the data
            printf("Processing data: %s\n", receivedPacket.data);

            // Increment the expected sequence number
            expectedSeqNum++;

            // Send acknowledgment for the received packet
            struct Packet ackPacket;
            ackPacket.sequenceNumber = receivedPacket.sequenceNumber;
