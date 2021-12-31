#include <pcap.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/ip.h>
#include <netinet/ether.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

void print_payload(const u_char *payload, int len);

void got_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet)
{
    struct sockaddr_in src;
    struct sockaddr_in dest;
    const char *payload;
    int payload_size;

    printf("Got a packet\n");
    const struct iphdr *ip = (struct iphdr *)(packet + ETH_HLEN);
    int ip_h_length = ip->ihl * 4;
    if (ip->protocol == IPPROTO_TCP)
    {
        const struct tcphdr *tcp = (struct tcphdr *)(packet + ETH_HLEN + ip_h_length);
        int tcp_h_length = tcp->doff * 4;
        payload = (u_char *)(packet + ETH_HLEN + ip_h_length + tcp_h_length);
        payload_size = ntohs(ip->tot_len) - (ip_h_length + tcp_h_length);
        memset(&src, 0, sizeof(src));
        src.sin_addr.s_addr = ip->saddr;
        memset(&dest, 0, sizeof(dest));
        dest.sin_addr.s_addr = ip->daddr;

        if (payload_size > 0)
        {
            printf("Source IP: %s\n", inet_ntoa(src.sin_addr));
            printf("Destination IP: %s\n", inet_ntoa(dest.sin_addr));
            printf("Protocol: TCP\n");
            print_payload(payload, payload_size);
        }
    }
    else
    {
        printf("Not TCP");
    }
}

int main()
{
    pcap_t *handle;
    char errbuf[PCAP_ERRBUF_SIZE];
    struct bpf_program fp;
    char filter_exp[] = "tcp";
    bpf_u_int32 net;

    handle = pcap_open_live("br-e8e039a3c88a", BUFSIZ, 1, 1000, errbuf);

    pcap_compile(handle, &fp, filter_exp, 0, net);
    if (pcap_setfilter(handle, &fp) != 0)
    {
        pcap_perror(handle, "Error:");
        exit(EXIT_FAILURE);
    }

    pcap_loop(handle, -1, got_packet, NULL);

    pcap_close(handle);
    return 0;
}

void print_payload(const u_char *payload, int len)
{
    const u_char *ch;
    ch = payload;
    printf("Payload: \n\t\t");

    for (int i = 0; i < len; i++)
    {
        if (isprint(*ch))
        {
            if (len == 1)
            {
                printf("\t%c", *ch);
            }
            else
            {
                printf("%c", *ch);
            }
        }
        ch++;
    }
    printf("\n____________________________________________\n");
}