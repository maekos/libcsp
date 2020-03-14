#ifndef CSP_IF_ETH_H_
#define CSP_IF_ETH_H_

#include <csp/csp.h>
#include <csp/arch/csp_thread.h>

/**
 * Setup ETH peer
 *
 * RX task:
 *   A server task will attempt at binding to ip 0.0.0.0 port 9600
 *   If this fails, it is because another eth server is already running.
 *   The server task will continue attemting the bind and will not exit before the application is closed.
 *
 * TX peer:
 *   Outgoing CSP packets will be transferred to the peer specified by the host argument
 */
int csp_if_eth_init();

CSP_DEFINE_TASK(csp_if_eth_rx_task);

int csp_if_eth_tx(csp_iface_t *ifc, csp_packet_t * packet, uint32_t timeout);

void csp_print_pkt_info(csp_packet_t *pkt);

void* csp_eth_basic_server(void *parameters); 
#endif /* CSP_IF_ETH_H_ */
