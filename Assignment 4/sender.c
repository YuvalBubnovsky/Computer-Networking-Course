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
#define SIZE 1048576 // 1024 * 1024 * 100 = 100MB of data
#define MTU 1500

// void send_file(FILE *fp, int sock); // declaring this function for later use, no header file needed here

int main()
{
    // creating a TCP socket
    int sock;
    sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    int rval = inet_pton(AF_INET, (const char *)ADDRESS, &server_address.sin_addr);
    if (rval <= 0)
    {
        perror("rvel");
        return -1;
    }

    // setting up buffer for CC algo switch later on
    char buf[256];
    socklen_t len;

    // defining file pointers
    FILE *fp;
    char *filename = "100mb.txt";
    char buffer[MTU];

    // connecting to server and making sure connection is succuessful
    int conn_status = connect(sock, (struct sockaddr *)&server_address, sizeof(server_address));
    if (conn_status < 0)
    {
        perror("conn_status");
        printf("\n");
        exit(1);
    }
    printf("Connected!, status: %d\n", conn_status);

    len = sizeof(buf);
    if (getsockopt(sock, IPPROTO_TCP, TCP_CONGESTION, buf, &len) != 0)
    {
        perror("getsockopt");
        printf("\n");

        return -1;
    }

    printf("Current: %s\n", buf);

    // Sending the file 5 times using the Cubic CC algorithm
    fp = fopen(filename, "r");
    if (fp == NULL)
    {
        perror("fopen");
        printf("\n");

        exit(1);
    }

    // converts file content to string:
    fseek(fp, 0, SEEK_END);
    size_t lngth = ftell(fp);

    // TODO: REMOVE //////////////
    printf("%lu bytes", lngth); ///
    //////////////////////////////

    fseek(fp, 0, SEEK_SET);
    char *messasge = (char *)malloc(lngth + 1);
    lngth = fread(messasge, 1, lngth, fp);
    fclose(fp);
    messasge[lngth] = '\0';
    lngth += 1; // TODO: is this needed? run with and without
    printf("\nMessasge length:%ld", sizeof(messasge));
    printf("\nMessage is:\n%s\n", messasge);

    // Start of send segment
    long sent = 0;
    // long messageLength = SIZE;
    // recall socket is 'sock'
    int packLen, j, r;
    long i;
    for (r = 0; r < 5; r++)
    {
        bzero(buffer, sizeof(buffer));
        packLen = 0;
        for (i = 0; i < lngth; i += packLen)
        {
            for (; i + j < lngth && j < sizeof(buffer); j++)
            {
                buffer[j] = messasge[i + j];
            }
            packLen = strlen(buffer);
            printf("%d\n", packLen);
            if (packLen == 0)
            {
                i = lngth;
                continue;
                // THIS EFFECTIVELY 'BREAKS' THE LOOP!
            }
            sent += send(sock, buffer, packLen, 0);
            j = 0;
        }
        // TODO: try to run without ////////////
        buffer[0] = '\0';
        buffer[MTU - 1] = '\0';
        send(sock, buffer, packLen, 0);

        printf("\n sent message %d with size %ld \n", r, sent);
        //////////////////////////////////////
    }

    // END of send segment

    // send_file(fp, sock);
    printf("Sent Data 1 times using cubic CC algorithm");
    printf("\n");

    // Switching the CC algorithm to be Reno
    // The code bit for switching algorithms is courtesy of StackOverflow
    strcpy(buf, "reno");
    len = strlen(buf);
    if (setsockopt(sock, IPPROTO_TCP, TCP_CONGESTION, buf, len) != 0)
    {
        perror("setsockopt");
        printf("\n");

        return -1;
    }
    len = sizeof(buf);
    if (getsockopt(sock, IPPROTO_TCP, TCP_CONGESTION, buf, &len) != 0)
    {
        perror("getsockopt");
        printf("\n");

        return -1;
    }
    printf("New: %s\n", buf);

    // Sending the file 5 times using the Reno CC algorithm
    /*fp = fopen(filename, "r");
    if (fp == NULL)
    {
        perror("fopen");
        printf("\n");

        exit(1);
    }
    for (int i = 0; i < 5; i++)
    {
        fscanf(fp, "%c", buffer);
        write(sock, buffer, 1500);
    }*/
    // send_file(fp, sock);
    printf("Sent Data 5 times using reno CC algorithm");
    printf("\n");

    // Closing the socket
    close(sock);
    return 0;
}
