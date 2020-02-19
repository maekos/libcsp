#include <stdlib.h>
#include <stdint.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <pthread.h>
#include <csp/drivers/udp.h>
#include <csp/csp.h>

struct sockaddr_in server_addr;

int udp_send(void *data, uint8_t length) {
    int sockfd;
    int broadcast = 1;
    struct sockaddr_in addr = {0};

    addr.sin_family = AF_INET;
    addr.sin_port = htons(9600);
    addr.sin_addr.s_addr = htonl(INADDR_BROADCAST);

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0 ) {
	    csp_log_error("Socket creation failed");
	    return CSP_ERR_BUSY;
    }
    
    if(setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcast,sizeof(broadcast)) < 0) {
	csp_log_error("Error in setting Broadcast option");
	close(sockfd);
	return 0;
    }
    
    sendto(sockfd, data, length, MSG_CONFIRM, (struct sockaddr *) &addr, sizeof(addr));
    close(sockfd);

    return CSP_ERR_NONE;
}

static void * socketudp_rx_thread(void * parameters)
{
	int sockfd;
	int nbytes;
	char buffer[300];
   	int broadcast = 1;

	if ((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0 ) {
		csp_log_error("Socket creation failed");
		return NULL;
	}
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(9600);

    if(setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast)) < 0)
    {
       	csp_log_error("Error in setting Broadcast option");
	       close(sockfd);
	       return 0;
    }
	while(1) {

		if (bind(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
			csp_log_warn("UDP server waiting for port 9600");
			sleep(1);
			continue;
		}

		while(1) {

			// TODO set bufsize
			nbytes = recv(sockfd, (void *)buffer, 100, MSG_WAITALL);
			csp_log_info("Packet received %d bytes", nbytes);

			/* Check for short */
			if (nbytes < 4) {
				csp_log_error("Too short UDP packet: %d", nbytes);
				continue;
			}


			csp_packet_t * packet = csp_buffer_get(100);

			if (packet == NULL)
				continue;

			memcpy(&packet->id, buffer, nbytes);
			packet->length = nbytes;
			csp_if_udp_rx(packet);
		}

	}

	pthread_exit(NULL);
}

int udp_init()
{
	pthread_t rx_thread;

	/* Create receive thread */
	if (pthread_create(&rx_thread, NULL, socketudp_rx_thread, NULL) != 0) {
		csp_log_error("pthread_create error");
		return -1;
	}

}
