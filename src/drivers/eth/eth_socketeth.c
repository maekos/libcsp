#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <net/ethernet.h>
#include <linux/if_ether.h>


#include <pthread.h>
#include <csp/drivers/eth.h>
#include <csp/csp.h>

#define CSP_PROTO 0xfefe
#define ETHER_TYPE 0x80ab /*  custom type */
#define ETH_FRAME_OVERHEAD 14 // dest_mac, src_mac, type, payload
#define BUF_SIZE     1024

static struct ifreq if_idx;
static struct ifreq if_mac;

static void print_eth_header(struct ethhdr * eth)
{
	printf("\nEthernet Header\n");
	printf("\t|-Source Address : %.2X-%.2X-%.2X-%.2X-%.2X-%.2X\n",eth->h_source[0],eth->h_source[1],eth->h_source[2],eth->h_source[3],eth->h_source[4],eth->h_source[5]);
	printf("\t|-Destination Address : %.2X-%.2X-%.2X-%.2X-%.2X-%.2X\n",eth->h_dest[0],eth->h_dest[1],eth->h_dest[2],eth->h_dest[3],eth->h_dest[4],eth->h_dest[5]);
	printf("\t|-Protocol : %d\n",eth->h_proto);
}

static void get_index(int sockfd)
{
    /* Get the index of the interface to send on */
    memset(&if_idx, 0, sizeof(struct ifreq));
    strncpy(if_idx.ifr_name, "eth0", IFNAMSIZ - 1);
    if (ioctl(sockfd, SIOCGIFINDEX, &if_idx) < 0) {
        csp_log_error("SIOCGIFINDEX");
        return;
    }
}

static void get_src_mac(int sockfd, const char * ifc)
{
    /* Get the MAC address of the interface to send on */
    memset(&if_mac, 0, sizeof(struct ifreq));
    strncpy(if_mac.ifr_name, ifc, IFNAMSIZ - 1);
    if (ioctl(sockfd, SIOCGIFHWADDR, &if_mac) < 0) {
        csp_log_error("SIOCGIFHWADDR");
        return;
    }
}

int eth_send(void *data, uint32_t len)
{
    int sockfd;
    int nbytes;
    char *sendbuf;
    struct ethhdr *eh;
    struct sockaddr_ll address;
    int tx_len = len + sizeof(struct ethhdr);

    /*  Open RAW socket to send on */
    if ((sockfd = socket(AF_PACKET, SOCK_RAW, htons(CSP_PROTOCOL))) == -1) {
        csp_log_info("socket");
    }
	
	/*  Set socket to allow broadcast */
    int broadcastPermission = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, (void *) &broadcastPermission, sizeof(broadcastPermission)) < 0)
		printf("setsockopt() failed");

	get_index(sockfd);
	get_src_mac(sockfd, "eth0");

	sendbuf = (char *) malloc(tx_len);
	memset(sendbuf, 0, tx_len);
    
	eh = (struct ethhdr *) sendbuf;

	/*  Ethernet header */
   	eh->h_source[0] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[0];
	eh->h_source[1] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[1];
	eh->h_source[2] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[2];
	eh->h_source[3] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[3];
	eh->h_source[4] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[4];
	eh->h_source[5] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[5];
	memset(eh->h_dest, 0xff, ETH_ALEN);
	/*  Ethertype field */
	eh->h_proto = htons (CSP_PROTO);
	memcpy(sendbuf+sizeof(struct ethhdr), data, len);

	print_eth_header(eh);

	/*  Index of the network device */
   	address.sll_ifindex = if_idx.ifr_ifindex;
	address.sll_family = PF_PACKET;
	/*  Address length*/
   	address.sll_halen = ETH_ALEN;

	/*  Destination MAC */
	memset(address.sll_addr, 0xff, ETH_ALEN);

	/*  Send packet */
   	nbytes = sendto(sockfd, sendbuf, tx_len, 0, (struct sockaddr*)&address, sizeof(struct sockaddr_ll));
   	if (nbytes < 0)
		printf("Send failed\n");
	printf("Sended %d bytes\n", nbytes);

    return CSP_ERR_NONE;
}

static void * socketeth_rx_thread(void * parameters)
{

	int sock_r;
	unsigned char buffer[BUF_SIZE];
	int nbytes;
	struct sockaddr saddr;
	int saddr_len = sizeof (saddr);
	unsigned char* csp_data;

	sock_r = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));

	if(sock_r < 0)
	{
		printf("error in socket\n");
		return NULL;
	}

	while(1)
	{	
		memset(buffer, 0, BUF_SIZE);
	 
		//Receive a network packet and copy in to buffer
		nbytes = recvfrom(sock_r, buffer, BUF_SIZE, 0, &saddr, (socklen_t *)&saddr_len);
		if(nbytes < 0)
		{
			printf("error in reading recvfrom function\n");
			continue;
		}

    	struct ethhdr *eh = (struct ethhdr *) buffer;
		//print_eth_header(eh);

		if(eh->h_proto != CSP_PROTO)
		{
			continue;
		}
		csp_log_info("CSP packet received: %d bytes", nbytes);

		csp_packet_t * packet = csp_buffer_get(100);

		if (packet == NULL)
			continue;

		csp_data = (unsigned char*) buffer + sizeof(struct ethhdr);
		memcpy(&packet->id, csp_data, nbytes - sizeof(struct ethhdr));

		// RX callback
		csp_if_eth_rx(packet);

	}	
	pthread_exit(NULL);
}

int eth_init()
{
	pthread_t rx_thread;

	/* Create receive thread */
	if (pthread_create(&rx_thread, NULL, socketeth_rx_thread, NULL) != 0) {
		csp_log_error("pthread_create error");
		return -1;
	}

}
