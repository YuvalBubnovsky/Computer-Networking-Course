#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <stdbool.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

#include <netinet/in.h>
#include <netinet/tcp.h>

#include <sys/time.h>

#define PORT 6769
#define ADDRESS "127.0.0.1"
#define SIZE 1048576

int main()
{
    // Setting up variables for later use
    int conn_status;
    int sock, sock_recv;
    struct sockaddr_in server_addr;
    socklen_t length;

    char buf[256]; // for CC algorithm change

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        perror("socket");
    }

    // Default CC algorithm is Cubic, we just make sure this is the correct now
    // So there will be no issues with recieving from sender.

    length = sizeof(buf);
    if (getsockopt(sock, IPPROTO_TCP, TCP_CONGESTION, buf, &length) != 0)
    {
        perror("getsockopt");
        return -1;
    }

    length = sizeof(buf);
    if (getsockopt(sock, IPPROTO_TCP, TCP_CONGESTION, buf, &length) != 0)
    {
        perror("getsockopt");
        return -1;
    }
    printf("Congestion Control Strategy: %s\n", buf);

    // Setting server_addr memory to 0 in order to make sure we open a fresh port without overlapping
    // any existing data
    memset(&server_addr, 0, sizeof(server_addr));

    // Setting up socket data
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    conn_status = bind(sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (conn_status < 0)
    {
        perror("bind");
        printf("\n");
        close(sock);
        exit(1);
    }

    conn_status = listen(sock, 200); // Ready to recieve 200 connections at the same time
    if (conn_status < 0)
    {
        perror("listen");
        printf("\n");
        close(sock);
        exit(1);
    }
    else
    {
        printf("Server listening..");
        printf("\n");
    }

    /*
    Next block of code is setting up variables to measure time as requested in the assignment,
    We will be using the <sys/time.h> library to do all the required calculations.
    Afterwards, we will switch to Reno CC algorithm and take the same measurements.
    */

    struct timeval start_time, end_time;
    double average_time_cubic;
    if (1 == 1)
    {
        sock_recv = accept(sock, NULL, NULL);
        char buffer[1500];
        if (sock_recv < 0)
        {
            perror("accept");
            close(sock);
            return -1;
        }
        gettimeofday(&start_time, NULL);
        int bytes_recv = recv(sock_recv, buffer, 1500, 0);
        if (bytes_recv < 0)
        {
            perror("recv");
        }
        // Recieving user data as long as there is something to recieve
        while (bytes_recv > 0)
        {
            bytes_recv = recv(sock_recv, buffer, 1500, 0);
        }
        gettimeofday(&end_time, NULL);
        // Time calculation function, took this function from - https://www.geeksforgeeks.org/how-to-measure-time-taken-by-a-program-in-c/
        average_time_cubic = ((end_time.tv_sec + end_time.tv_usec) / 1e6) - ((start_time.tv_sec - start_time.tv_usec) / 1e6);
        close(sock_recv);
    }

    // Changing CC algorithm to Reno

    strcpy(buf, "reno");
    length = strlen(buf);
    if (setsockopt(sock, IPPROTO_TCP, TCP_CONGESTION, buf, length) != 0)
    {
        perror("setsockopt");
        return -1;
    }

    length = sizeof(buf);

    if (getsockopt(sock, IPPROTO_TCP, TCP_CONGESTION, buf, &length) != 0)
    {
        perror("getsockopt");
        return -1;
    }
    printf("New Congestion Control Strategy: %s\n", buf);

    // Same logic as above for measuring average time, this time for Reno CC algorithm
    double average_time_reno = 0;
    if (1 == 1)
    {
        int sock_recv = accept(sock, NULL, NULL);
        char buffer[1500];
        if (sock_recv < 0)
        {
            perror("accept");
            close(sock);
            return -1;
        }
        gettimeofday(&start_time, NULL);
        int bytes_recv = recv(sock_recv, buffer, 1500, 0);
        if (bytes_recv < 0)
        {
            perror("recv");
        }
        // Recieving user data as long as there is something to recieve
        while (bytes_recv > 0)
        {
            bytes_recv = recv(sock_recv, buffer, 1500, 0);
        }
        gettimeofday(&end_time, NULL);
        // Time calculation function, took this function from - https://www.geeksforgeeks.org/how-to-measure-time-taken-by-a-program-in-c/
        average_time_cubic = ((end_time.tv_sec + end_time.tv_usec) / 1e6) - ((start_time.tv_sec - start_time.tv_usec) / 1e6);
        close(sock_recv);
    }
    average_time_cubic = average_time_cubic / 5;
    average_time_reno = average_time_reno / 5;
    printf("=== Summery Of Average Time: ===");
    printf("Cubic CC algorithm: %f seconds\n", average_time_cubic);
    printf("Reno CC algorithm: %f seconds\n", average_time_reno);
    close(sock);
    return 0;
}