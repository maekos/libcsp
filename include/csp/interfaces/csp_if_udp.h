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

#ifndef CSP_IF_UDP_H_
#define CSP_IF_UDP_H_

#include <csp/csp.h>
#include <csp/arch/csp_thread.h>

csp_iface_t csp_if_udp;
/**
 * Setup UDP peer
 *
 * RX task:
 *   A server task will attempt at binding to ip 0.0.0.0 port 9600
 *   If this fails, it is because another udp server is already running.
 *   The server task will continue attemting the bind and will not exit before the application is closed.
 *
 * TX peer:
 *   Outgoing CSP packets will be transferred to the peer specified by the host argument
 */
int csp_if_udp_init();

CSP_DEFINE_TASK(csp_if_udp_rx_task);

int csp_if_udp_tx(csp_iface_t *ifc, csp_packet_t * packet, uint32_t timeout);

void csp_print_pkt_info(csp_packet_t *pkt);

void* csp_udp_basic_server(void *parameters); 
#endif /* CSP_IF_UDP_H_ */
