#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <csp/csp.h>
#include <csp/csp_interface.h>
#include <csp/csp_endian.h>
#include <csp/drivers/udp.h>
#include <csp/interfaces/csp_if_udp.h>


int csp_if_udp_tx(csp_iface_t *ifc, csp_packet_t * packet, uint32_t timeout) {
	int ret;
	ret = udp_send((void *) &packet->id, packet->length);
	return ret;
}

int csp_if_udp_rx(csp_packet_t* packet)
{
	csp_new_packet(packet, &csp_if_udp, NULL);
}

int csp_if_udp_init()
{
	/* MTU is datasize */
	csp_if_udp.mtu = csp_buffer_datasize();
	/* Register interface */
	csp_if_udp.name = "UDP";
	csp_if_udp.nexthop = csp_if_udp_tx;

	csp_iflist_add(&csp_if_udp);

	csp_route_set(CSP_DEFAULT_ROUTE, &csp_if_udp, CSP_NODE_MAC);

	/* Init buffer system with 10 packets of maximum 300 bytes each */
	csp_buffer_init(20, 300);

	udp_init();
	csp_log_info("Server UDP task started");

	return CSP_ERR_NONE;
}

