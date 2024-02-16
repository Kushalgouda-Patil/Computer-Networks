#include "headerFiles.h"
#define BUFFER_SIZE 5
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
    FILE *fp;

    fp = fopen("input.txt", "r");
    if (fp == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    
    
    movePtr = sendBuffer;
    struct pollfd fds[1];
    fds[0].fd = sd;
    fds[0].events = POLLIN;

    
    fseek(fp, 0, SEEK_END);
    int totalChars = ftell(fp);
    int totalFrames=totalChars/3;
    printf("Total frames: %d\n",++totalFrames);
    fseek(fp, 0, SEEK_SET);
    for(int i=0;i<totalFrames;i++)
    {
        int pollRes=poll(fds, 1, 5000);
        if(pollRes==-1)
        {
            perror("poll error");
        }
        else if(pollRes==0)
        {
            printf("Timeout\n");
            printf("Sending lost frame %d\n",--i);
            send(sd, sendBuffer, BUFFER_SIZE, 0);
            continue;
        }
        else 
        {
            printf("Data available\n");
            recv(sd, (void *)&recvBufferInt, sizeof(recvBufferInt), 0);
            if(recvBufferInt==(i%2))
            {
                printf("ACK %d received\n",i);
                
                fgets(movePtr+1, BUFFER_SIZE-1, fp);
                if(fp==NULL)
                {
                    printf("End of file\n");
                    break;
                }
                sendBuffer[0]=i%2;
                send(sd, sendBuffer, BUFFER_SIZE, 0);
                printf("Sent %d\n",i);
            }
            else
            {
                printf("ACK %d delayed\n",recvBufferInt);
                i--;
                continue;
            }
        }
        
    
        
        }
        
        fclose(fp);
        close(sd);

        return 0;
    }
