
#include "headerFiles.h"

typedef struct{
    int seq_no;
    char data[5];
    bool isAcked;
}Frame;
int main(void) {

    int listensd, connectsd, n, processID;
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
            Frame frames[20];
            int start=0;
            int ack=0;
            Frame tempframe;
            for(start=0;start<20;)
            {
                n=recv(connectsd,&tempframe,sizeof(tempframe),0);
                if(n==-1)
                {
                    perror("recv");
                    close(connectsd);
                    exit(EXIT_FAILURE);
                }
                else if(n==0)
                {
                    printf("Client disconnected\n");
                    close(connectsd);
                    exit(0);
                }
                else if(tempframe.seq_no==start%8)
                {
                    printf("Received frame %d\n",tempframe.seq_no);
                    int rv=readStdinTimeout(5);
                    if(rv>0)
                    {
                        fscanf(stdin,"%d",&ack);
                        
                        if(ack==0)
                        {
                            ack=7;
                        }
                        else
                        {
                            ack--;
                        }
                        send(connectsd,&ack,sizeof(ack),0);
                    }
                    else
                    {
                        printf("Timeout\n");
                        continue;
                    }
                    if(ack==tempframe.seq_no)
                    {
                        frames[start].seq_no=tempframe.seq_no;
                        strcpy(frames[start].data,tempframe.data);
                        frames[start].isAcked=true;
                        FILE *fp;
                        fp=fopen("output.txt","a");
                        fprintf(fp,"%s",frames[start].data);
                        fclose(fp);
                        start++;
                    }
                    
                }
                else
                {
                    printf("discard frame %d\n",tempframe.seq_no);
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
