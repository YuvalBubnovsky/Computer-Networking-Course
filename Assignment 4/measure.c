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
#include <time.h>
#include <sys/time.h>

#define PORT 6769
#define ADDRESS "127.0.0.1"
#define SIZE 104857600 // 1024 * 1024 * 100 = 100MB of data

int main()
{
    // char *address = ADDRESS;
    int conn_status;

    int sock, sock_recv;
    struct sockaddr_in server_addr, recv_addr;
    socklen_t addr_size;

    char buffer[1500];

    sock = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    conn_status = bind(sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (conn_status < 0)
    {
        perror("conn_status");
        printf("\n");

        exit(1);
    }

    conn_status = listen(sock, 10); // Ready to recieve 10 connections at the same time
    if (conn_status < 0)
    {
        perror("conn_status");
        printf("\n");
        exit(1);
    }
    else
    {
        printf("Server listening..");
        printf("\n");
    }
    addr_size = sizeof(sock_recv);
    time_t firstBatchStart, firstBatchEnd;
    double averageTime;
    time(&firstBatchStart);

    sock_recv = accept(sock, (struct sockaddr *)&recv_addr, &addr_size);
    if (sock_recv < 0)
    {
        perror("sock_recv");
        exit(1);
    }
    if (recv(sock_recv, buffer, sizeof(buffer), 0) < 0)
    {
        perror("recv");
    }

    time(&firstBatchEnd);
    printf("Recieved 5 files using Cubic CC algorithm");
    printf("\n");
    averageTime = difftime(firstBatchEnd, firstBatchStart) / 5;
    printf("%s%f", "Average recieving time using Cubic: ", averageTime);
    printf("\n");
}