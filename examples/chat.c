#include <stdlib.h>
#include <stdio.h>
#include <csp/csp.h>
#include <csp/arch/csp_thread.h>
#include "chat.h"

static void fill_packet(unsigned int * node, unsigned int * port,  unsigned char * data)
{
		*node = 1;
		*port = 22;
		memcpy((char *) data, "Hola Mundo!", strlen("Hola Mundo!"));
}

static void fill_packet_from_stdin(unsigned int * node, unsigned int * port,  unsigned char * data)
{
		printf("Nodo destino: ");
		fflush(stdout);
		scanf("%d", node);
		printf("Puerto destino: ");
		fflush(stdout);
		scanf("%d", port);
		getchar();
		printf("Data: ");
		fflush(stdout);
		fgets((char *)data, 100, stdin);
}

void chat()
{
	unsigned int node;
	unsigned int port;

	csp_packet_t * packet;
	/* Client */
	packet = csp_buffer_get(100);
	/* Get packet buffer for data */
	if (packet == NULL) {
	/* Could not get buffer element */
		csp_log_error("[Client] Failed to get buffer element");
		return;
	}

	while(1)
	{
		/* Here you can choose send to default or manual fill */
		//fill_packet(&node, &port, packet->data);
		fill_packet_from_stdin(&node, &port, packet->data);

    	packet->length = strlen((char *)packet->data) + CSP_HEADER_LENGTH;

		/*  Connect to host HOST, port PORT with regular UDP-like protocol and 1000 ms timeout */
    	csp_conn_t *conn = csp_connect(CSP_PRIO_NORM, node, port, 1000, CSP_O_NONE);
    	if (conn == NULL) 
		{
			/*  Connect failed */
			csp_log_info("Connection failed");
			/*  Remember to free packet buffer */
			csp_buffer_free(packet);
			return;
		}

		/*  Send packet */
    	if (!conn || !csp_send(conn, packet, 1000))
		{
			/*  Send failed */
			csp_log_info("Send failed");
			csp_buffer_free(packet);
		}
		csp_close(conn);
    	csp_sleep_ms(1000);
	}
}
