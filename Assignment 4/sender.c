#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>

#include <netinet/in.h>
#include <netinet/tcp.h>

#include <unistd.h>
#include <arpa/inet.h>

#include <fcntl.h>

#define PORT 6769
#define ADDRESS "127.0.0.1"
#define SIZE 104857600 // 1024 * 1024 * 100 = 100MB of data

void send_file(FILE *fp, int sock); // declaring this function for later use, no header file needed here

int main()
{
    // creating a TCP socket
    int sock;
    sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = inet_addr(ADDRESS);

    // setting up buffer for CC algo switch later on
    char buf[256];
    socklen_t len;

    // setting the socket to be non-blocking
    fcntl(sock, F_SETFL, O_NONBLOCK);

    // defining file pointers
    FILE *fp;
    char *filename = "100mb.txt";

    // connecting to server and making sure connection is succuessful
    int conn_status = connect(sock, (struct sockaddr *)&server_address, sizeof(server_address));
    if (conn_status < 0)
    {
        printf("%s", "ERROR: Connection was NOT succuessful");
        exit(1);
    }

    len = sizeof(buf);
    if (getsockopt(sock, IPPROTO_TCP, TCP_CONGESTION, buf, &len) != 0)
    {
        perror("getsockopt");
        return -1;
    }

    printf("Current: %s\n", buf);

    // Sending the file 5 times using the Cubic CC algorithm
    for (int i = 0; i <= 4; i++)
    {
        fp = fopen(filename, "r");
        if (fp == NULL)
        {
            printf("ERROR: File read unsuccuessful");
            exit(1);
        }
        send_file(fp, sock);
    }
    printf("Sent Data 5 times using cubic CC algorithm");

    // Switching the CC algorithm to be Reno
    // The code bit for switching algorithms is courtesy of StackOverflow
    strcpy(buf, "reno");
    len = strlen(buf);
    if (setsockopt(sock, IPPROTO_TCP, TCP_CONGESTION, buf, len) != 0)
    {
        perror("setsockopt");
        return -1;
    }
    len = sizeof(buf);
    if (getsockopt(sock, IPPROTO_TCP, TCP_CONGESTION, buf, &len) != 0)
    {
        perror("getsockopt");
        return -1;
    }
    printf("New: %s\n", buf);

    // Sending the file 5 times using the Reno CC algorithm
    for (int i = 0; i <= 4; i++)
    {
        fp = fopen(filename, "r");
        if (fp == NULL)
        {
            printf("ERROR: File read unsuccuessful");
            exit(1);
        }
        send_file(fp, sock);
    }
    printf("Sent Data 5 times using reno CC algorithm");

    // Closing the socket
    close(sock);
    return 0;
}

void send_file(FILE *fp, int sock)
{
    char data[SIZE] = {0};
    while (fgets(data, SIZE, fp) != NULL)
    {
        if (send(sock, data, sizeof(data), 0) == -1)
        {
            printf("ERROR: File send unsuccuessful");
            exit(1);
        }
        bzero(data, SIZE);
    }
}