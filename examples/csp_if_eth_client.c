#include <stdio.h>
#include <stdlib.h>
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
#include <csp/interfaces/csp_if_eth.h>

#include "chat.h"

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

int main(int argc, char **argv) {

	argparse(argc, argv);
	csp_debug_toggle_level(2);
	csp_log_info("[Client] Initialising CSP node %d", node);
	csp_init(node);
	csp_if_eth_init();

	chat();

    return 0;
}
