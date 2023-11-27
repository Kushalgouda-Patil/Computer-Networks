#include <stdio.h>
#include <conio.h>
#include <winsock2.h>
#include <time.h>
#pragma comment(lib, "ws2_32.lib")
void timer(int);

int main(int argc, char *argv[])
{
    WSADATA wsa;
    SOCKET sck;
    int cnt;
    struct sockaddr_in ip_info;
    char server_reply[2000];
    int recv_size;
    char msg[100];
    printf("Installing");
    for (cnt = 1; cnt <= 3; cnt++)
    {
        timer(1);
        printf(".");
    }

    if (WSAStartup(MAKEWORD(2, 2), &wsa) == INVALID_SOCKET)
        printf("WSA error\n");
    else if ((sck = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
        printf("Socket error\n");
    else
    {
        ip_info.sin_addr.s_addr = inet_addr("127.0.0.1");
        ip_info.sin_family = AF_INET;
        ip_info.sin_port = htons(8888);
    }

    if (connect(sck, (struct sockaddr *)&ip_info, sizeof(ip_info)) < 0)
        printf("\nNot connected\n");
    else
        printf("\nSession is up\n");

    while (1)
    {
        printf("Me: ");
        gets(msg);
        send(sck, msg, strlen(msg), 0);
        if ((recv_size = recv(sck, server_reply, 2000, 0)) == SOCKET_ERROR)
            puts("recv failed");
        else
        {
            server_reply[recv_size] = '\0';
            puts(server_reply);
        }
    }
    getch();
    return 0;
}
// Timer function:
void timer(int sec)
{
    clock_t end;
    end = clock() + sec * CLOCKS_PER_SEC;
    while (clock() <= end)
        ;
}