#include <arpa/inet.h> // htons
#include<unistd.h> //getpid
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
#define DESTMAC0 0x90
#define DESTMAC1 0x94
#define DESTMAC2 0xe4
#define DESTMAC3 0xfc
#define DESTMAC4 0x52
#define DESTMAC5 0x85
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
typedef struct add{
	unsigned char Name;
	unsigned short Type;
	unsigned short UDP_Payload_Size;
	unsigned char HBIER;	// higher bit in extended Rcode
	unsigned char EDNS0_V;
	unsigned short Z;
	unsigned short data_length;
}Additional_Records;	
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
int main() {
	char interface_name[] = "enp0s8";
	unsigned char hostname[100] = "google.com";
	int sock;
	sock = socket(AF_PACKET, SOCK_RAW, IPPROTO_RAW);
	if (sock == -1)
		printf("Socket is down\n");
	else {
		printf("\nStage1: Create Socket Completed!\n\n");
		// start of getting interface index
		struct ifreq ifreq_idx;

		memset(&ifreq_idx, 0, sizeof(ifreq_idx));

		strncpy(ifreq_idx.ifr_name, interface_name,
				IFNAMSIZ - 1); // if no need to change interface ,5 is enough?
		printf("\n The interface name is: %s\n", ifreq_idx.ifr_name);

		if ((ioctl(sock, SIOCGIFINDEX, &ifreq_idx)) < 0)
			printf("\nioctl read index fail\n");

		printf("\nThe network interface index is %d\n", ifreq_idx.ifr_ifindex);

		// get the interface index

		// start of getting MAC address
		struct ifreq ifreq_mac;

		memset(&ifreq_mac, 0, sizeof(ifreq_mac));

		strncpy(ifreq_mac.ifr_name, interface_name, IFNAMSIZ - 1);

		if ((ioctl(sock, SIOCGIFHWADDR, &ifreq_mac)) < 0)
			printf("\nioctl fails 2 times\n");

		// get the interface  MAC Address

		//  start of getting interface IP Address
		struct ifreq ifreq_ip;

		memset(&ifreq_ip, 0, sizeof(ifreq_ip));
		strncpy(ifreq_ip.ifr_name, interface_name, IFNAMSIZ - 1);

		if (ioctl(sock, SIOCGIFADDR, &ifreq_ip) < 0)
			printf("\nioctl fails 3 times\n");

		// get the interface IP Address

		// here we construct the ethernet header
		int total = 0;

		unsigned char *sendbuff;
		sendbuff = (unsigned char *)malloc(128);
		memset(sendbuff, 0, 128);

		struct ether_header *eth = (struct ether_header *)(sendbuff);

		eth->ether_shost[0] = (unsigned char)(ifreq_mac.ifr_hwaddr.sa_data[0]);
		eth->ether_shost[1] = (unsigned char)(ifreq_mac.ifr_hwaddr.sa_data[1]);
		eth->ether_shost[2] = (unsigned char)(ifreq_mac.ifr_hwaddr.sa_data[2]);
		eth->ether_shost[3] = (unsigned char)(ifreq_mac.ifr_hwaddr.sa_data[3]);
		eth->ether_shost[4] =
			(unsigned char)(ifreq_mac.ifr_hwaddr.sa_data[4]); // source mac address
		eth->ether_shost[5] = (unsigned char)(ifreq_mac.ifr_hwaddr.sa_data[5]);
		// /*
		eth->ether_dhost[0] = DESTMAC0;
		eth->ether_dhost[1] = DESTMAC1;
		eth->ether_dhost[2] = DESTMAC2;
		eth->ether_dhost[3] = DESTMAC3;
		eth->ether_dhost[4] = DESTMAC4;
		eth->ether_dhost[5] = DESTMAC5;
		//               we fill destination mac address here */
		eth->ether_type = htons(ETH_P_IP); // next header is IP header
		total += sizeof(struct ether_header);
		// construct an ip header
		struct iphdr *iph =
			(struct iphdr *)(sendbuff + sizeof(struct ether_header));
		iph->ihl = 5;     // ip header length
		iph->version = 4; // ipv4
		iph->tos = 16;    // type Of Service // low delay
		iph->id = htons(21910);
		iph->ttl = 64;
		iph->protocol = 17; // 17 for UDP
		iph->saddr = inet_addr("140.113.65.126");
//		iph->saddr = inet_addr("192.168.0.111");
		iph->daddr = inet_addr("8.8.8.8");
		total += sizeof(struct iphdr);
		sendbuff[15] = 0;
		struct udphdr *uh =
			(struct udphdr *)(sendbuff + sizeof(struct ether_header) +
					sizeof(struct iphdr));

		uh->source = htons(42210);
		uh->dest = htons(53);
		uh->check = 0;

		total += sizeof(struct udphdr);

		struct DNS_HEADER *DNS =
			(struct DNS_HEADER *)(sendbuff + sizeof(struct ether_header) +
					sizeof(struct iphdr) + sizeof(udphdr));

		DNS->id = (unsigned short) htons(getpid());

		DNS->qr = 0;     // this is a query instead of response;

		DNS->opcode = 0; // this is a standard query;

		DNS->aa = 0;     // Not Authoritative

		DNS->tc = 0;     // This message is not truncated

		DNS->rd = 1;     // recursion desired

		DNS->ra = 0;     // Recursion is not available.

		DNS->z = 0;      // z!


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
			(unsigned char *)(sendbuff + sizeof(struct ether_header) +
					sizeof(struct iphdr) + sizeof(udphdr) +
					sizeof(DNS_HEADER));
		DNSNameFormat(q_name, hostname);
		printf("\n");

		total += (strlen((const char *)q_name) + 1);
		struct QUESTION *qinfo;
		qinfo = (struct QUESTION *)(sendbuff + sizeof(struct ether_header) +
				sizeof(struct iphdr) + sizeof(udphdr) +
				sizeof(DNS_HEADER) +
				(strlen((const char *)q_name) + 1));
		qinfo->type = htons(query_type);
		qinfo->qclass = htons(1);

		total += (sizeof(struct QUESTION));

		unsigned char Name = 0x00;
		unsigned char  Type1 = 0x00;
		unsigned char  Type2 = 0x29;
		unsigned char UDP_Payload_Size1 = 0x10;
		unsigned char UDP_Payload_Size2 = 0x00;
		unsigned char HBIER=0x00;	// higher bit in extended Rcode
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
/*
		Add->Name = 0x31;
		Add->Type = 0x2923;
		Add->UDP_Payload_Size = 0x8794;
		Add->HBIER = 0x39;
		Add->EDNS0_V = 0x94;
		Add->Z = 0x5708;
		Add->data_length = 0x9185;
		printf("Name: %lu\n",sizeof(Add->Name));
		printf("Type: %lu\n",sizeof(Add->Type));
		printf("UDP_Payload_Size: %lu\n",sizeof(Add->UDP_Payload_Size));
		printf("HBIER: %lu\n",sizeof(Add->HBIER));
		printf("EDNS0_V: %lu\n",sizeof(Add->EDNS0_V));
		printf("Z: %lu\n",sizeof(Add->Z));
		printf("data_length: %lu\n",sizeof(Add->data_length));
		total += sizeof(Additional_Records);       
*/
		uh->len =
			htons((total - sizeof(struct ether_header) - sizeof(struct iphdr)));
		iph->tot_len = htons(total - sizeof(struct ether_header));

		iph->check =
			checksum((unsigned short *)(sendbuff + sizeof(struct ether_header)),
					(sizeof(struct iphdr) / 2));
		unsigned char rec;
		rec = sendbuff[24];
		sendbuff[24] = sendbuff[25];
		sendbuff[25] = rec;
		printf("\nAfter udp : %d\n", total);

		struct sockaddr_ll saddr_ll;
		saddr_ll.sll_ifindex = ifreq_idx.ifr_ifindex;
		saddr_ll.sll_halen = ETH_ALEN; // ETH_ALEN is defined 6
		saddr_ll.sll_addr[0] = DESTMAC0;
		saddr_ll.sll_addr[1] = DESTMAC1;
		saddr_ll.sll_addr[2] = DESTMAC2;
		saddr_ll.sll_addr[3] = DESTMAC3;
		saddr_ll.sll_addr[4] = DESTMAC4;
		saddr_ll.sll_addr[5] = DESTMAC5;
		printf("\nAt last: %d\n", total);
		int send_len =
			sendto(sock, sendbuff, total, 0, (struct sockaddr *)&saddr_ll,
					sizeof(struct sockaddr_ll));
		if (send_len < 0)
			printf("\nsendto error\n");
		printf("\n succeed! \n");
	}
}
