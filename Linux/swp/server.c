
#include "headerFiles.h"
#define BUFFER_SIZE 5
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
            FILE *fp;
            fp = fopen("output.txt", "w");
            fseek(fp, 0, SEEK_SET);
            int ack=0;
            send(connectsd, (void *)&ack, sizeof(ack), 0);
            for(;;)
            {
                fp = fopen("output.txt", "a");
                bytesToRecv = BUFFER_SIZE;
                movePtr = buffer;
                n=recv(connectsd, movePtr, bytesToRecv, 0);
                if (n == -1) {
                    perror("recv");
                    close(connectsd);
                    exit(EXIT_FAILURE);
                }
                printf("%d%s\n", buffer[0],&buffer[1]);
                int rv=readStdinTimeout(5);
                if(rv>0)
                {
                    fscanf(stdin,"%d",&ack);
                    if(ack==-1)
                        break;
                    send(connectsd, (void *)&ack, sizeof(ack), 0);
                    int fw=fwrite(buffer+1, sizeof(char), n-2, fp);
                    printf("Wrote %d bytes\n",fw);
                    fclose(fp);

                }
                else
                {
                    // --i;
                    continue;
                }
                
            }
            
            close(connectsd);
            exit(0);
        } else if (processID == -1) {
            perror("fork");
        } else {
            close(connectsd);
        }
    }

    close(listensd);  // Close server socket outside the loop

    return 0;
}
