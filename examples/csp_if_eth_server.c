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
#include <csp/interfaces/csp_if_eth.h>

#include "csp_basic_server.h"


static unsigned int node;

static void usage()
{
	printf("----------------------------------\n");
	printf("This is the help for CSP Server\n"
	       "Options are:\n"
	       "    -n: Node number for this host\n");
}

static void argparse(int argc, char** argv)
{
    int opt; 
    // put ':' in the starting of the 
    // string so that program can
    // distinguish between '?' and ':'  
    while((opt = getopt(argc, argv, "n:")) != -1)
    {
    	switch(opt)
		{
	    	case 'n':
	    		node = atoi(optarg);
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
	argparse(argc, argv);
	
	csp_debug_toggle_level(2);
	pthread_t rx_thread;
	csp_log_info("Initialising CSP node: %d", node);
	csp_init(node);
	csp_if_eth_init();

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

