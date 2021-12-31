#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>

unsigned short calculate_checksum(unsigned short *paddress, int len)
{
  int nleft = len;
  int sum = 0;
  unsigned short *w = paddress;
  unsigned short answer = 0;

  while (nleft > 1)
  {
    sum += *w++;
    nleft -= 2;
  }

  if (nleft == 1)
  {
    *((unsigned char *)&answer) = *((unsigned char *)w);
    sum += answer;
  }

  // add back carry outs from top 16 bits to low 16 bits
  sum = (sum >> 16) + (sum & 0xffff); // add hi 16 to low 16
  sum += (sum >> 16);                 // add carry
  answer = ~sum;                      // truncate to 16 bits

  return answer;
}

void send_raw_ip_packet(struct iphdr *ip)
{
  struct sockaddr_in dest_info;
  int enable = 1;

  // Step 1: Create a raw network socket.
  int sock = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);

  // Step 2: Set socket option.
  setsockopt(sock, IPPROTO_IP, IP_HDRINCL,
             &enable, sizeof(enable));

  // Step 3: Provide needed information about destination.
  dest_info.sin_family = AF_INET;
  dest_info.sin_addr.s_addr = ip->daddr;

  // Step 4: Send the packet out.
  sendto(sock, ip, ntohs(ip->tot_len), 0,
         (struct sockaddr *)&dest_info, sizeof(dest_info));
  close(sock);
}

int main()
{
  char buffer[1500];

  memset(buffer, 0, 1500);

  /*********************************************************
      Step 1: Fill in the ICMP header.
    ********************************************************/
  struct icmphdr *icmp = (struct icmphdr *)(buffer + sizeof(struct iphdr));
  icmp->type = 8; //ICMP Type: 8 is request, 0 is reply.

  // Calculate the checksum for integrity
  icmp->checksum = 0;
  icmp->checksum = calculate_checksum((unsigned short *)icmp,
                                      sizeof(struct icmphdr));

  /*********************************************************
      Step 2: Fill in the IP header.
    ********************************************************/
  struct iphdr *ip = (struct iphdr *)buffer;
  ip->version = 4;
  ip->ihl = 5;
  ip->ttl = 20;
  ip->saddr = inet_addr("1.2.3.4");
  ip->daddr = inet_addr("10.0.2.4");
  ip->protocol = IPPROTO_ICMP;
  ip->tot_len = htons(sizeof(struct iphdr) +
                      sizeof(struct icmphdr));

  /*********************************************************
      Step 3: Finally, send the spoofed packet
    ********************************************************/
  send_raw_ip_packet(ip);

  return 0;
}