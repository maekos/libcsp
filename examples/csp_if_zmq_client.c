#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdint.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>

#include <csp/csp.h>
#include <csp/csp_endian.h>
#include <csp/csp_interface.h>
#include <csp/arch/csp_thread.h>
#include <csp/interfaces/csp_if_zmqhub.h>

static unsigned int csp_node;  // This node
static unsigned int dest_node; // Destination node

static char subscriber[50];
static char publisher[50];

static void usage()
{
	printf("----------------------------------\n");
	printf("This is the help for CSP zmq client\n"
	       "Options are:\n"
	       "    -n: CSP Node number for this host\n"
	       "    -s: IP:port address for subscriber host\n"
	       "    -p: IP:port address for publisher host\n"
	       "    -d: Destination node\n"
	       );
}

static void argparse(int argc, char** argv)
{
    int opt; 
      
    while((opt = getopt(argc, argv, "n:d:s:p:")) != -1)
    {  
        switch(opt)  
        {  
	    case 'n':
	    	csp_node = atoi(optarg);
	    	break;

	    case 'd':
	    	dest_node = atoi(optarg);
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

int main(int argc, char **argv) {
    int pingResult;
    pthread_t rx_thread;

    argparse(argc, argv);

    csp_debug_toggle_level(2);
    csp_buffer_init(20, 300);
    csp_init(csp_node);

    csp_zmqhub_init_w_endpoints(255, publisher, subscriber);
    /* Regsiter interface */
    csp_iflist_add(&csp_if_zmqhub);
    csp_route_set(CSP_DEFAULT_ROUTE, &csp_if_zmqhub, CSP_NODE_MAC);
    csp_route_start_task(500, 0);
    csp_sleep_ms(1000); // is needeed for zmq stablish connections
    while(1) 
    {
       pingResult = csp_ping(dest_node, 3000, 10, CSP_O_NONE);
	   printf("Ping to %d, took %d ms\n", dest_node, pingResult);
	   csp_sleep_ms(1000);
    }
    return 0;
}
