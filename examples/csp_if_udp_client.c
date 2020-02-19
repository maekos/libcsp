/*
Cubesat Space Protocol - A small network-layer protocol designed for Cubesats
Copyright (C) 2012 GomSpace ApS (http://www.gomspace.com)
Copyright (C) 2012 AAUSAT3 Project (http://aausat3.space.aau.dk)

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdint.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <csp/csp.h>
#include <csp/csp_endian.h>
#include <csp/csp_interface.h>
#include <csp/arch/csp_thread.h>
#include <csp/interfaces/csp_if_udp.h>

unsigned int node, port;

static void fill_packet(unsigned int *node, unsigned int *port, unsigned char* data)
{
		*node = 1;
		*port = 22;
		memcpy((char *) data, "Hola Mundo!", strlen("Hola Mundo!"));
}

static void fill_packet_from_stdin(unsigned int * node, unsigned int * port, unsigned char * data)
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

void ping()
{
    int pingResult;

    for(int i = 50; i <= 200; i+= 50) {
        pingResult = csp_ping(1, 3000, 10, CSP_O_NONE);
        printf("Ping with payload of %d bytes, took %d ms\n", i, pingResult);
        csp_sleep_ms(1000);
    }
}

void chat()
{
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
		// fill_packet(&node, &port, packet->data);
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

int main(int argc, char **argv) {

	csp_debug_toggle_level(2);
	csp_log_info("[Client] Initialising CSP");
	csp_init(atoi(argv[1]));
	csp_if_udp_init();

	chat();
	//ping();

    return 0;
}
