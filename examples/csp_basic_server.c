#include <stdlib.h>
#include <stdio.h>
#include <csp/csp.h>
#include <csp/arch/csp_thread.h>
#include "csp_basic_server.h"

#define CHAT_PORT 22

void* csp_basic_server(void *parameters) 
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
    csp_packet_t *response;

    /* Process incoming connections */
    while (1)
    {
        /* Wait for connection, 10000 ms timeout */
        conn = csp_accept(sock, 10000);
        if (!conn)
            continue;

        /* Read packets. Timeout is 1000 ms */
        while ((packet = csp_read(conn, 1000)) != NULL) 
        {
            switch (csp_conn_dport(conn)) 
            {
            case CHAT_PORT:
                /* Process packet here */
                printf("From[%d]: ", packet->id.src);
                printf("%s\n", packet->data);
                break;
            default:
                /* Let the service handler reply pings, buffer use, etc. */
                csp_service_handler(conn, packet);
                break;
            }
        }

        /* Close current connection, and handle next */
        csp_close(conn);
    }
}
