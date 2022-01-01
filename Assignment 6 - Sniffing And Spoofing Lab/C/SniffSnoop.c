#include <pcap.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netinet/ether.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

void send_echo_reply(const struct iphdr *ip);
void send_raw_ip_packet(struct iphdr *ip);

// Loopback function for pcap
void got_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet)
{
    struct sockaddr_in src;
    struct sockaddr_in dest;

    printf("Got a packet\n");
    const struct iphdr *ip = (struct iphdr *)(packet + ETH_HLEN);
    if (ip->protocol == IPPROTO_ICMP)
    {
        memset(&src, 0, sizeof(src));
        src.sin_addr.s_addr = ip->saddr;
        memset(&dest, 0, sizeof(dest));
        dest.sin_addr.s_addr = ip->daddr;

        printf("Source IP: %s\n", inet_ntoa(src.sin_addr));
        printf("Destination IP: %s\n", inet_ntoa(dest.sin_addr));
        printf("Protocol: ICMP\n");
        send_echo_reply(ip);
    }
    else
    {
        printf("Not ICMP");
    }
}

void send_raw_ip_packet(struct iphdr *ip)
{
    struct sockaddr_in dest_info;
    int enable = 1;

    // Creating the raw socket
    int sock = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);

    // Enabling promiscous mode on the socket
    setsockopt(sock, IPPROTO_IP, IP_HDRINCL, &enable, sizeof(enable));

    dest_info.sin_family = AF_INET;
    dest_info.sin_addr.s_addr = ip->daddr;

    sendto(sock, ip, ntohs(ip->tot_len), 0, (struct sockaddr *)&dest_info, sizeof(dest_info));
    close(sock);
}

void send_echo_reply(const struct iphdr *ip)
{
    int ip_h_len = ip->ihl * 4;
    const char buffer[512];

    // make a copy from original packet to buffer (fake packet)
    memset((char *)buffer, 0, 512);
    memcpy((char *)buffer, ip, ntohs(ip->tot_len));
    struct iphdr *fake_ip = (struct iphdr *)buffer;
    struct icmphdr *fake_icmp = (struct icmphdr *)(buffer + ip_h_len);

    // Swap source & destination from the recieved packet into the fake one
    fake_ip->saddr = ip->daddr;
    fake_ip->daddr = ip->saddr;
    fake_ip->ttl = 64;
    fake_icmp->type = 0; // type 0 is echo reply

    send_raw_ip_packet(fake_ip);
}

int main()
{
    pcap_t *handle;
    char errbuf[PCAP_ERRBUF_SIZE];
    struct bpf_program fp;
    char filter_exp[] = "icmp";
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