/*
Cubesat Space Protocol - A small network-layer protocol designed for Cubesats

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
#include <sys/socket.h>
#include <arpa/inet.h>

#include <csp/csp.h>
#include <csp/csp_interface.h>
#include <csp/csp_endian.h>
#include <csp/drivers/eth.h>
#include <csp/interfaces/csp_if_eth.h>


int csp_if_eth_tx(csp_iface_t *ifc, csp_packet_t * packet, uint32_t timeout) {
	int ret;
	ret = eth_send((void *) &packet->id, packet->length);
	return ret;
}

int csp_if_eth_rx(csp_packet_t* packet)
{
	csp_new_packet(packet, &csp_if_eth, NULL);
}

int csp_if_eth_init()
{
	/* MTU is datasize */
	csp_if_eth.mtu = csp_buffer_datasize();
	/* Register interface */
	csp_if_eth.name = "ETH";
	csp_if_eth.nexthop = csp_if_eth_tx;

	csp_iflist_add(&csp_if_eth);

	csp_route_set(CSP_DEFAULT_ROUTE, &csp_if_eth, CSP_NODE_MAC);

	/* Init buffer system with 10 packets of maximum 300 bytes each */
	csp_buffer_init(20, 300);

	eth_init();
	csp_log_info("Server ETH task started");

	return CSP_ERR_NONE;
}

void csp_print_pkt_info(csp_packet_t *packet) {
	printf("\n------------------------------\n");
	printf("Length: %d\n", packet->length);
	printf("Src: %d\n", packet->id.src);
	printf("Dest: %d\n", packet->id.dst);
	printf("Src port: %d\n", packet->id.sport);
	printf("Dst port: %d\n", packet->id.dport);
	printf("Payload: %s\n", packet->data);
	printf("------------------------------\n");
}
