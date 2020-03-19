#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <errno.h>
#include <stdint.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <getopt.h>

#include <csp/csp.h>
#include <csp/csp_endian.h>
#include <csp/csp_interface.h>
#include <csp/arch/csp_thread.h>
#include <csp/interfaces/csp_if_udp.h>
#include <csp/interfaces/csp_if_eth.h>


static void usage()
{
	printf("----------------------------------\n");
	printf("This is the help for CSP Server\n"
	       "Options are:\n"
	       "    --help: display what you are reading now\n"
	       "    -n: Node number for this host\n"
	       "    -p: Port number for this host\n");
}

static void argparse(int argc, char** argv, int* node)
{
    int opt; 
      
    // put ':' in the starting of the 
    // string so that program can  
    //distinguish between '?' and ':'  
	while((opt = getopt(argc, argv, "n:")) != -1)
    {  
        switch(opt)  
        {  
			case 'n':
				*node = atoi(optarg);
				break;
        }      
	}
}


void* csp_router(void *parameters) 
{
    /* Create socket without any socket options */
    csp_socket_t *sock = csp_socket(CSP_SO_NONE);
	if(!sock)
	{
		csp_log_info("No se pudo crear el socket");
		return NULL;
	}

    /* Bind all ports to socket */
    csp_bind(sock, CSP_ANY);

    /* Create 10 connections backlog queue */
    csp_listen(sock, 10);

    /* Pointer to current connection and packet */
    csp_conn_t *conn;
    csp_packet_t *packet;

    /* Process incoming connections */
    while (1) 
	{
        /* Wait for connection, 10000 ms timeout */
        conn = csp_accept(sock, 10000);
        if (!conn)
		{
            continue;
		}

        /* Read packets. Timeout is 1000 ms */
        while ((packet = csp_read(conn, 1000)) != NULL) 
		{
			csp_log_info("Me llego un paquete para rutear!");
        	csp_service_handler(conn, packet);
            }

        /* Close current connection, and handle next */
        csp_close(conn);
    }
}


int main(int argc, char **argv) 
{
	int node;
	argparse(argc, argv, &node);
		
	csp_debug_toggle_level(2);
	pthread_t rx_thread;

	csp_log_info("[Master] Initialising CSP node: %d", node);
	csp_init(node);
	csp_if_eth_init();
	csp_if_udp_init();
	csp_route_start_task(500, 1);


	csp_route_set(31, &csp_if_eth, CSP_NODE_MAC);
	csp_route_set(32, &csp_if_udp, CSP_NODE_MAC);
	csp_log_info("CSP Router adapter task started");

	if (pthread_create(&rx_thread, NULL, csp_router, NULL) != 0) {
		csp_log_error("pthread_create: %s", strerror(errno));
		return -1;
	}

	/* Wait for execution to end (ctrl+c) */
	while(1) {
		printf("\n------------------------------\n");
		//csp_conn_print_table();
		csp_route_print_table();
		csp_route_print_interfaces();
        	csp_sleep_ms(10000);
    	}
    
   	return 0;
}

