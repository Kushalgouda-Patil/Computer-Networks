#ifndef header_file
#define header_file
#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<netdb.h>
#include<errno.h>
#include<sys/stat.h>
#include<signal.h>
#include<fcntl.h>
#include<sys/poll.h>
#include<poll.h>
#include<sys/time.h>
#include<sys/select.h>

int readStdinTimeout(int sec)
{
    fd_set set;
    struct timeval timeout;
    int rv;

    // Set the timeout to 5 seconds
    timeout.tv_sec = sec;
    timeout.tv_usec = 0;
    // Set stdin as the file descriptor to monitor
    FD_ZERO(&set);
    FD_SET(fileno(stdin), &set);
    rv = select(fileno(stdin) + 1, &set, NULL, NULL, &timeout);
    return rv;
}
#endif