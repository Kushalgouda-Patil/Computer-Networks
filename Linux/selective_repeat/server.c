
#include "headerFiles.h"
#define BUFFER_SIZE 5
#define WINDOW_SIZE 4
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
            char ** window=malloc(sizeof(char*)*WINDOW_SIZE);
            for(int i=0;i<WINDOW_SIZE;i++)
            {
                memset(buffer,0,sizeof(buffer));
                n=recv(connectsd,buffer,sizeof(buffer),0);
                window[buffer[0]]=malloc(sizeof(buffer));
                strcpy(window[buffer[0]],buffer);
                if(n==-1){
                    perror("recv");
                    exit(EXIT_FAILURE);
                }
                if(n==0){
                    break;
                }
                fwrite(buffer,1,n,fp);
            }
            printwindow(window,WINDOW_SIZE);
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
