/*! @file socket_client.h
 * @brief stuff related to socket clients (ie, connecting to socket servers)
 */

#pragma once

#include "socket.h"
#include "multiaddr/multiaddr.h"
#include "utils/logger.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>

/*! @typedef socket_client
 * @struct socket_client
 * a generic tcp/udp socket client
 */
typedef struct socket_client {
    int socket_number;
    addr_info *peer_address;
} socket_client_t;

/*! @brief returns a new socket client connected to `addr:port`
  * @param thl an instance of a thread logger
  * @param addr the multiaddr to connect to
 */
socket_client_t *new_socket_client(thread_logger *thl, multi_addr_t *addr);

/*!
 * @brief used to send a message through the connected socket number
 * @param client an instance of socket_client_t created with new_socket_client
 * @param peer_address the target address to connect to through the socket
 * @param message a null terminated pointer to a char
 * @returns Success: 0
 * @returns Failure: 1
 */
int socket_client_sendto(socket_client_t *client, addr_info *peer_address,
                         char *message);