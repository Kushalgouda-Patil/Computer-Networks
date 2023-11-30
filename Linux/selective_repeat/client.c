#include "headerFiles.h"
#define BUFFER_SIZE 5
#define WINDOW_SIZE 4
int main(void) {
    int sd, n, bytesToRecv;
    char sendBuffer[BUFFER_SIZE], recvBuffer[BUFFER_SIZE];
    int recvBufferInt;
    char* movePtr;
    struct sockaddr_in serverAddress;

    sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

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
    
    FILE *fp=fopen("input.txt","r");
    if(fp==NULL){
        perror("fopen");
        exit(EXIT_FAILURE); 
    }
    

        fclose(fp);

        return 0;
    }
