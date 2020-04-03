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
#include <csp/interfaces/csp_if_zmqhub.h>

#include "csp_basic_server.h"


static unsigned int csp_node;

static char subscriber[50];
static char publisher[50];

static void usage()
{
	printf("----------------------------------\n");
	printf("This is the help for CSP zmq client\n"
	       "Options are:\n"
	       "    -n: CSP Node number for this host\n"
	       "    -h: IP address for publisher host\n"
	       "    -p: Port for publisher host\n"
	       );
}

static void argparse(int argc, char** argv)
{
    int opt; 
      
    while((opt = getopt(argc, argv, "n:s:p:")) != -1)
    {  
        switch(opt)  
        {  
	    case 'n':
	    	csp_node = atoi(optarg);
	    	break;

	    case 's':
		strcpy(subscriber, optarg);
	    	break;

	    case 'p':
		strcpy(publisher, optarg);
	    	break;

	    case '?':
		if (optopt == 'c')
			fprintf (stderr, "Option -%c requires an argument.\n", optopt);
		else if (isprint (optopt))
			fprintf (stderr, "Unknown option `-%c'.\n", optopt);
		else
			fprintf (stderr,"Unknown option character `\\x%x'.\n", optopt);
		usage();
        }      
    }
}

int main(int argc, char **argv) 
{
	pthread_t rx_thread;

	argparse(argc, argv);
	
	csp_debug_toggle_level(2);
	csp_log_info("Initialising CSP node: %d", csp_node);
	csp_init(csp_node);
	csp_buffer_init(20, 300);
	csp_iflist_add(&csp_if_zmqhub);

	csp_zmqhub_init_w_endpoints(255, publisher, subscriber);
	csp_route_set(CSP_DEFAULT_ROUTE, &csp_if_zmqhub, CSP_NODE_MAC);

	csp_route_start_task(500, 1);

	if (pthread_create(&rx_thread, NULL, csp_basic_server, NULL) != 0) 
	{
	    csp_log_error("pthread_create: %s", strerror(errno));
	    return -1;
	}

	csp_log_info("Server task started");

	while(1) 
	{
        csp_sleep_ms(10000);
    }
   	return 0;
}

