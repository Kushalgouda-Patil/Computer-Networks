#include "headerFiles.h"
#define BUFFER_SIZE 2
typedef struct{
    int seq_no;
    char data[5];
    bool isAcked;
}Frame;
int start=0,end=0;
void printFrame(Frame frames[],int fsize)
{
    for(int i=0;i<fsize;i++)
    {
        printf("%d---%s \n",frames[i].seq_no,frames[i].data);
    }

}
void sendframes(int sd,Frame frames[])
{
    while(end-start<5)
    {
        send(sd,&frames[end],sizeof(frames[end]),0);
        printf("Sent frame %d\n",frames[end].seq_no);
        end=(end+1);
    }
}
void resendframes(int sd,Frame frames[])
{
    for(int i=start;i<end;i++)
    {
        send(sd,&frames[i],sizeof(frames[i]),0);
        printf("Resent frame %d\n",frames[i].seq_no);
    }
}
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
    Frame frames[20];
    FILE *fp;
    fp=fopen("input.txt","r");
    int seq_no=0;
    for(int i=0;i<20;i++)
    {
        frames[i].seq_no=seq_no;
        frames[i].isAcked=false;
        if(fgets(frames[i].data,5,fp)==NULL)
        {
            frames[i].data[0]='\0';
        }
        seq_no=(seq_no+1)%8;

    }
    printFrame(frames,20);
    int ack;
    struct pollfd fds[1];
    fds[0].fd = sd;
    fds[0].events = POLLIN;
    while(end<20)
    {
        sendframes(sd,frames);
        int pollRes=poll(fds, 1, 5000);
        // printf("%d",pollRes);
        if(pollRes==-1)
        {
            perror("poll error");
        }
        else if(pollRes==0)
        {
            printf("Timeout\n");
            
            resendframes(sd,frames);
        }
        else
        {
            recv(sd,&ack,sizeof(ack),0);
            printf("Received ack %d\n",ack);
            if(ack==frames[start].seq_no)
            {
                frames[start].isAcked=true;
                start=(start+1);
            }
            else
            {
                resendframes(sd,frames);
            }

        }
        
    }
    printf("All frames sent\n");
    fclose(fp);
    

    
        return 0;
    }
