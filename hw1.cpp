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
#include <sys/socket.h> // this for struct sockaddr, socket ,and AF_PACKET
#include <unistd.h>     //getpid
//#define DESTMAC0 0x90
//#define DESTMAC1 0x94
//#define DESTMAC2 0xe4
//#define DESTMAC3 0xfc
//#define DESTMAC4 0x52
//#define DESTMAC5 0x85
#define query_type 1
// DNS HEADER Structure
struct DNS_HEADER {
  unsigned short id; // identification

  unsigned char rd : 1;     // recursion desired
  unsigned char tc : 1;     // truncated message
  unsigned char aa : 1;     // authoritive answer
  unsigned char opcode : 4; // purpose of the message
  unsigned char qr : 1;     // query/response flag

  unsigned char rcode : 4; // response code
  unsigned char cd : 1;    // checking disabled
  unsigned char ad : 1;    // authenticated data
  unsigned char z : 1;     // its z! reserved
  unsigned char ra : 1;    // recursion available

  unsigned short q_count;    // number of question entries
  unsigned short ans_count;  // number of answer entries
  unsigned short auth_count; // number of autority entries
  unsigned short add_count;  // number of resource entries
};
typedef struct add {
  unsigned char Name;
  unsigned short Type;
  unsigned short UDP_Payload_Size;
  unsigned char HBIER; // higher bit in extended Rcode
  unsigned char EDNS0_V;
  unsigned short Z;
  unsigned short data_length;
} Additional_Records;
struct QUESTION {
  unsigned short type;
  unsigned short qclass;
};
unsigned short checksum(unsigned short *buff, int nwords) {
  unsigned long sum;
  for (sum = 0; nwords > 0; nwords--)
    sum += htons(*(buff)++);

  sum = ((sum >> 16) + (sum & 0xFFFF));
  sum += (sum >> 16);
  return (unsigned short)(~sum);
}
void DNSNameFormat(unsigned char *DNS, unsigned char *host) {
  int lock = 0, i;
  strcat((char *)host, ".");

  for (i = 0; i < strlen((char *)host); i++) {
    if (host[i] == '.') {
      *DNS++ = i - lock;
      for (; lock < i; lock++) {
        *DNS++ = host[lock];
      }
      lock++;
    }
  }
  *DNS++ = '\0';
}
int main(int argc, char **argv) {
  unsigned char hostname[100] = "google.com";
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

    uh->source = htons(38210);
    uh->dest = htons(53);
    uh->check = 0;

    total += sizeof(struct udphdr);

    struct DNS_HEADER *DNS =
        (struct DNS_HEADER *)(sendbuff + sizeof(struct iphdr) + sizeof(udphdr));

    DNS->id = (unsigned short)htons(getpid());

    DNS->qr = 0; // this is a query instead of response;

    DNS->opcode = 0; // this is a standard query;

    DNS->aa = 0; // Not Authoritative

    DNS->tc = 0; // This message is not truncated

    DNS->rd = 1; // recursion desired

    DNS->ra = 0; // Recursion is not available.

    DNS->z = 0; // z!

    DNS->ad = 1;

    DNS->cd = 0;

    DNS->rcode = 0;

    DNS->q_count = htons(1); // one question;

    DNS->ans_count = 0;

    DNS->auth_count = 0;

    DNS->add_count = htons(1);

    printf("\nDNS: %lu\n", sizeof(struct DNS_HEADER));
    total += sizeof(struct DNS_HEADER);
    printf("\nAfter DNS : %d\n", total);
    unsigned char *q_name =
        (unsigned char *)(sendbuff + sizeof(struct iphdr) + sizeof(udphdr) +
                          sizeof(DNS_HEADER));
    DNSNameFormat(q_name, hostname);
    printf("\n");

    total += (strlen((const char *)q_name) + 1);
    struct QUESTION *qinfo;
    qinfo = (struct QUESTION *)(sendbuff + sizeof(struct iphdr) +
                                sizeof(udphdr) + sizeof(DNS_HEADER) +
                                (strlen((const char *)q_name) + 1));
    qinfo->type = htons(query_type);
    qinfo->qclass = htons(1);

    total += (sizeof(struct QUESTION));

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

    struct sockaddr_in sin, din;
    sin.sin_family = AF_INET;
    din.sin_family = AF_INET;

    sin.sin_port = htons(38210);
    din.sin_port = htons(53);

    sin.sin_addr.s_addr = inet_addr(argv[1]);
    din.sin_addr.s_addr = inet_addr(argv[2]);

    printf("\nAt last: %d\n", total);

    int send_len =
        sendto(sock, sendbuff, total, 0, (struct sockaddr *)&sin, sizeof(sin));
    if (send_len < 0)
      printf("\nsendto error\n");
    printf("\n succeed! \n");
  }
}
