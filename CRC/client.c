#include "headerFiles.h"
#define BUFFER_SIZE 15
#define N strlen(gen_poly)

char data[28];
char check_value[28];

char gen_poly[10]= "101"; 

int data_length, i, j;

void XOR(char *check_value)
{

    for (j = 1; j < N; j++)
        check_value[j] = ((check_value[j] == gen_poly[j]) ? '0' : '1');
}
void crc(char *data,char* check_value)
{
    
    for (i = 0; i < N; i++)
        check_value[i] = data[i];
    do
    {
        if (check_value[0] == '1')
            XOR(check_value);

        for (j = 0; j < N - 1; j++)
            check_value[j] = check_value[j + 1];
        
        check_value[j] = data[i++];
    } while (i <= data_length + N - 1);
    
}
void receiver(char *data,char* check_value)
{
    // get the received data
    printf("Enter the received data: ");
    scanf("%s", data);
    printf("\n-----------------------------\n");
    printf("Data received: %s", data);
    // Cyclic Redundancy Check
    crc(data,check_value);
    // Check if the remainder is zero to find the error
    for (i = 0; (i < N - 1) && (check_value[i] != '1'); i++)
        ;
    if (i < N - 1)
        printf("\nError detected\n\n");
    else
        printf("\nNo error detected\n\n");
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
    printf("Enter string to send\n");
    scanf("%s",sendBuffer);
    strcpy(data,sendBuffer);
    data_length = strlen(data);
    // appending n-1 zeros to the data
    for (i = data_length; i < data_length + N - 1; i++)
        data[i] = '0';
    printf("\n Data padded with n-1 zeros : %s", data);
    printf("\n----------------------------------------");
    crc(data,check_value);
    // print the computed check value
    printf("\nCRC or Check value is : %s", check_value);
     // Append data with check_value(CRC)
    for (i = data_length; i < data_length + N - 1; i++)
        data[i] = check_value[i - data_length];
    
    printf("\n Final data to be sent : %s", data);
    printf("\n----------------------------------------\n");
    send(sd, data, strlen(data), 0);    
        
        
    
    close(sd);

    return 0;
}
