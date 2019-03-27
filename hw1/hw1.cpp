#include <arpa/inet.h> // htons
#include <asm/sockios.h> // this is for SIOCGIF(Put the interface index into the ifreq structure)
#include <linux/if_packet.h>
#include <net/if.h> // struct ifreq
#include <netinet/ether.h>
#include <netinet/if_ether.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>  // ioctl
#include <sys/socket.h> // this for struct sockaddr, socket ,and AF_INET
#include <unistd.h>     //getpid
unsigned short checksum(unsigned short *buff, int nwords) {
  unsigned long sum;
  for (sum = 0; nwords > 0; nwords--)
    sum += htons(*(buff)++);

  sum = ((sum >> 16) + (sum & 0xFFFF));
  sum += (sum >> 16);
  return (unsigned short)(~sum);
}
int main(int argc, char **argv) {
  int one = 1;
  const int *val = &one;
  int sock;

  sock = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
  if (sock == -1)
    printf("Socket is down\n");
  else {
    int total = 0;

    unsigned char *sendbuff;
    sendbuff = (unsigned char *)malloc(128);
    memset(sendbuff, 0, 128);

    // construct an ip header
    struct iphdr *iph = (struct iphdr *)(sendbuff);
    iph->ihl = 5;     // ip header length
    iph->version = 4; // ipv4
    iph->tos = 16;    // type Of Service // low delay
    iph->id = htons(21910);
    iph->ttl = 64;
    iph->protocol = 17; // 17 for UDP
    iph->saddr = inet_addr(argv[1]);
    iph->daddr = inet_addr(argv[2]);
    total += sizeof(struct iphdr);
    struct udphdr *uh = (struct udphdr *)(sendbuff + sizeof(struct iphdr));

    uh->source = htons(34820);
    uh->dest = htons(53);
    uh->check = 0;

    total += sizeof(struct udphdr);

    unsigned short *i = (unsigned short *)(sendbuff + sizeof(struct iphdr) +
                                           sizeof(struct udphdr));

    unsigned char flag1 = 0x01;
    unsigned char flag2 = 0x20;

    unsigned char q_count1 = 0x00;
    unsigned char q_count2 = 0x01;
    unsigned char ans_count1 = 0x00;
    unsigned char ans_count2 = 0x00;

    unsigned char auth_count1 = 0x00;
    unsigned char auth_count2 = 0x00;
    unsigned char add_count1 = 0x00;
    unsigned char add_count2 = 0x01;

    *i = (unsigned short)htons(getpid());
    total += sizeof(unsigned short);
    sendbuff[total++] = flag1;
    sendbuff[total++] = flag2;
    sendbuff[total++] = q_count1;
    sendbuff[total++] = q_count2;
    sendbuff[total++] = ans_count1;
    sendbuff[total++] = ans_count2;
    sendbuff[total++] = auth_count1;
    sendbuff[total++] = auth_count2;
    sendbuff[total++] = add_count1;
    sendbuff[total++] = add_count2;

    unsigned char name1 = 0x02;
    unsigned char name2 = 0x75;
    unsigned char name3 = 0x73;
    unsigned char name4 = 0x03;
    unsigned char name5 = 0x6f;
    unsigned char name6 = 0x72;
    unsigned char name7 = 0x67;
    unsigned char name8 = 0x00;
    sendbuff[total++] = name1;
    sendbuff[total++] = name2;
    sendbuff[total++] = name3;
    sendbuff[total++] = name4;
    sendbuff[total++] = name5;
    sendbuff[total++] = name6;
    sendbuff[total++] = name7;
    sendbuff[total++] = name8;

    unsigned char DNS_Type1 = 0x00;
    unsigned char DNS_Type2 = 0xff;
    unsigned char DNS_class1 = 0x00;
    unsigned char DNS_class2 = 0x01;
    sendbuff[total++] = DNS_Type1;
    sendbuff[total++] = DNS_Type2;
    sendbuff[total++] = DNS_class1;
    sendbuff[total++] = DNS_class2;

    unsigned char Name = 0x00;
    unsigned char Type1 = 0x00;
    unsigned char Type2 = 0x29;
    unsigned char UDP_Payload_Size1 = 0x10;
    unsigned char UDP_Payload_Size2 = 0x00;
    unsigned char HBIER = 0x00; // higher bit in extended Rcode
    unsigned char EDNS0_V = 0x00;
    unsigned char Z1 = 0x00;
    unsigned char Z2 = 0x00;
    unsigned char data_length1 = 0x00;
    unsigned char data_length2 = 0x00;
    sendbuff[total++] = Name;
    sendbuff[total++] = Type1;

    sendbuff[total++] = Type2;
    sendbuff[total++] = UDP_Payload_Size1;
    sendbuff[total++] = UDP_Payload_Size2;
    sendbuff[total++] = HBIER;
    sendbuff[total++] = EDNS0_V;
    sendbuff[total++] = Z1;
    sendbuff[total++] = Z2;
    sendbuff[total++] = data_length1;
    sendbuff[total++] = data_length2;

    uh->len = htons((total - sizeof(struct iphdr)));

    iph->tot_len = htons(total);

    iph->check =
        checksum((unsigned short *)(sendbuff), (sizeof(struct iphdr) / 2));
    printf("\nAfter udp : %d\n", total);
    if ((setsockopt(sock, IPPROTO_IP, IP_HDRINCL, val, sizeof(one))) < 0)
      printf("\nsetsockopt error\n");

    struct sockaddr_in sin;
    sin.sin_family = AF_INET;

    sin.sin_port = htons(53);

    sin.sin_addr.s_addr = inet_addr(argv[2]);

    printf("\nAt last: %d\n", total);

    int send_len =
        sendto(sock, sendbuff, total, 0, (struct sockaddr *)&sin, sizeof(sin));
    if (send_len < 0)
      printf("\nsendto error\n");
    printf("\n succeed! \n");
  }
}
