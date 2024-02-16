
#include "headerFiles.h"
#include "util.h"
#define BUFFER_SIZE 15
#define N strlen(gen_poly)


// CRC value
char data[28];
char check_value[28];
// generator polynomial
char gen_poly[10]= "101"; 
// variables
int data_length, i, j;
// function that performs XOR operation
void XOR(char *check_value)
{
    // if both bits are the same, the output is 0
    // if the bits are different the output is 1
    for (j = 1; j < N; j++)
        check_value[j] = ((check_value[j] == gen_poly[j]) ? '0' : '1');
}
void crc(char *data,char* check_value)
{
    // initializing check_value
    for (i = 0; i < N; i++)
        check_value[i] = data[i];
    do
    {
        // check if the first bit is 1 and calls XOR function
        if (check_value[0] == '1')
            XOR(check_value);
        // Move the bits by 1 position for the next computation
        for (j = 0; j < N - 1; j++)
            check_value[j] = check_value[j + 1];
        // appending a bit from data
        check_value[j] = data[i++];
    } while (i <= data_length + N - 1);
    // loop until the data ends
}
void receiver(char *data,char* check_value)
{
    // get the received data
    printf("Enter the data(same for no error): ");
    scanf("%s", data);
    printf("\n-----------------------------\n");
    printf("Data received: %s", data);
    // Cyclic Redundancy Check
    crc(data,check_value);
    // Check if the remainder is zero to find the error
    for (i = 0; (i < N - 1) && (check_value[i] != '1'); i++)
        ;
    if (i < N - 1){
        printf("\nError detected\n\n");
        printf("%s",check_value);}
    else{
        printf("\nNo error detected\n\n");
        printf("%s",check_value);}
}
int main(void) {
    // printf("\n Enter the Generating polynomial: ");
    // // get the generator polynomial
    // scanf("%s", gen_poly);
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
            bytesToRecv = BUFFER_SIZE;
            movePtr = buffer;
            n=recv(connectsd, movePtr, bytesToRecv, 0);
            if (n == -1) {
                perror("recv");
                close(connectsd);
                exit(EXIT_FAILURE);
            }
            printf("Printing received data %s\n", buffer);
            char data[256];
            for(i=0;i<strlen(buffer)-strlen(gen_poly)+1;i++)
            {
                data[i]=buffer[i];
            }
            data[i]='\0';
            data_length=strlen(data);
            for (j=0;j<strlen(gen_poly)-1;j++)
            {
                check_value[j]=buffer[i++];
            }
            check_value[i]='\0';
            printf("\nCRC or Check value is : %s", check_value);
            printf("\n----------------------------------------\n");
            printf("\n Actual data received : %s", data);
            printf("\n----------------------------------------\n");

            receiver(data,buffer);
            
            
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
