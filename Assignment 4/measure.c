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

int main()
{
    char *address = ADDRESS;
    int conn_status;

    int sock, sock_recv;
    struct sockaddr_in server_addr, recv_addr;
    socklen_t addr_size;
    char buffer[SIZE];

    sock = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = PORT;
    server_addr.sin_addr.s_addr = inet_addr(ADDRESS);

    conn_status = bind(sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (conn_status < 0)
    {
        printf("ERROR: Binding error!");
        exit(1);
    }
    conn_status = listen(sock, 5);
    if (conn_status < 0)
    {
        printf("ERROR: Listen error!");
        exit(1);
    }
    else
    {
        printf("Server listening..");
    }
    addr_size = sizeof(sock_recv);
    sock_recv = accept(sock, (struct sockaddr *)&recv_addr, &addr_size);
}