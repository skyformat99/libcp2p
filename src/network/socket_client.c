/*! @file socket_client.c
 * @brief client socket related tooling
 */

#include "network/socket_client.h"
#include "utils/logger.h"
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#pragma GCC diagnostic ignored "-Wunused-parameter"
/*! @brief returns a new socket client connected to `addr:port`
  * @param thl an instance of a thread logger
  * @param addr the multiaddr to connect to
 */
socket_client_t *new_socket_client(thread_logger *thl, multi_addr_t *addr) {
    char ip[1024];

    int rc = multiaddress_get_ip_address(addr, ip);
    if (rc != 1) {
        return NULL;
    }
    int port = multiaddress_get_ip_port(addr);
    if (port == -1) {
        return NULL;
    }
    char cport[7];
    sprintf(cport, "%i", port);
    
    addr_info hints;
    memset(&hints, 0, sizeof(hints));
    bool is_tcp = false;
    bool is_udp = false;

    if (strstr(addr->string, "/tcp/") != NULL) {
        is_tcp = true;
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags = AI_PASSIVE;
    }
    if (strstr(addr->string, "/udp/") != NULL) {
        is_udp = true;
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_DGRAM;
        hints.ai_flags = AI_PASSIVE;
    }

    if (is_tcp == false && is_udp == false) {
        return NULL;
    }

    addr_info *peer_address;
    rc = getaddrinfo(ip, cport, &hints, &peer_address);
    if (rc != 0) {
        freeaddrinfo(peer_address);
        return NULL;
    }

    int client_socket_num = get_new_socket(thl, peer_address, NULL, 0, true);
    if (client_socket_num == -1) {
        thl->log(thl, 0, "failed to get_new_socket", LOG_LEVELS_ERROR);
        freeaddrinfo(peer_address);
        return NULL;
    }

    socket_client_t *sock_client = calloc(sizeof(sock_client), sizeof(sock_client) + sizeof(peer_address));
    if (sock_client == NULL) {
        thl->log(thl, 0, "failed to calloc socket_client_t", LOG_LEVELS_ERROR);
        return NULL;
    }
    sock_client->socket_number = client_socket_num;
    sock_client->peer_address = peer_address;

    thl->log(thl, 0, "client successfully created", LOG_LEVELS_INFO);

    return sock_client;
}

/*!
 * @brief used to send a message through the connected socket number
 * @param client an instance of socket_client_t created with new_socket_client
 * @param peer_address the target address to connect to through the socket
 * @param message a null terminated pointer to a char
 * @returns Success: 1
 * @returns Failure: 0
 */
int socket_client_sendto(socket_client_t *client, addr_info *peer_address,
                         char *message) {
    int bytes_sent = sendto(client->socket_number, message, strlen(message), 0,
                            peer_address->ai_addr, peer_address->ai_addrlen);
    if (bytes_sent == -1) {
        return 0;
    }
    /*! *@todo if we sent less than total size, send remaining
     */
    return 0;
}