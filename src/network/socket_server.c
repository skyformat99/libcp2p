/*! @file socket_server.c
 * @brief used to create a tcp/udp socket server listening on multiaddrs
 */

#include "network/socket_server.h"
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

/*! @brief returns a new socket server bound to the port number and ready to accept
 * connections
 */
socket_server_t *new_socket_server(thread_logger *thl,
                                   socket_server_config_t config) {

    SOCKET_OPTS opts[2] = {REUSEADDR, NOBLOCK};

    addr_info tcp_hints;
    addr_info udp_hints;
    addr_info *tcp_bind_address = NULL;
    addr_info *udp_bind_address = NULL;

    bool tcp_set = false;
    bool udp_set = false;

    int tcp_socket_num = 0;
    int udp_socket_num = 0;
    int rc = 0;

    if (config.tcp_port_number != NULL) {
        memset(&tcp_hints, 0, sizeof(tcp_hints));
        tcp_hints.ai_family = AF_INET;
        tcp_hints.ai_socktype = SOCK_STREAM;
        /*! @warning support non wildcard
         * @todo support non wildcard
         */
        tcp_hints.ai_flags = AI_PASSIVE;
        tcp_set = true;
    }

    if (config.udp_port_number != NULL) {
        memset(&udp_hints, 0, sizeof(udp_hints));
        udp_hints.ai_family = AF_INET;
        udp_hints.ai_socktype = SOCK_DGRAM;
        /*! @warning support non wildcard
         * @todo support non wildcard
         */
        udp_hints.ai_flags = AI_PASSIVE;
        udp_set = true;
    }

    // setup tcp socket
    if (tcp_set == true) {

        rc = getaddrinfo(0, config.tcp_port_number, &tcp_hints, &tcp_bind_address);
        if (rc != 0) {
            thl->log(thl, 0, "failed to get tcp addr info", LOG_LEVELS_ERROR);
            goto EXIT;
        }

        tcp_socket_num = get_new_socket(thl, tcp_bind_address, opts, 2);
        if (tcp_socket_num == -1) {
            thl->log(thl, 0, "failed to get new tcp socket", LOG_LEVELS_ERROR);
            goto EXIT;
        }

        listen(tcp_socket_num, config.max_connections);
        if (errno != 0) {
            thl->logf(thl, 0, LOG_LEVELS_ERROR,
                      "failed to start listening on tcp socket with error %s",
                      strerror(errno));
            goto EXIT;
        }

        thl->log(thl, 0, "accepting tcp connections", LOG_LEVELS_INFO);
    }

    // setup udp socket
    if (udp_set == true) {

        rc = getaddrinfo(0, config.tcp_port_number, &udp_hints, &udp_bind_address);
        if (rc != 0) {
            thl->log(thl, 0, "failed to get udp addr info", LOG_LEVELS_ERROR);
            goto EXIT;
        }

        udp_socket_num = get_new_socket(thl, udp_bind_address, opts, 2);
        if (udp_socket_num == -1) {
            thl->log(thl, 0, "failed to get new udp socket", LOG_LEVELS_ERROR);
            goto EXIT;
        }

        listen(udp_socket_num, config.max_connections);
        if (errno != 0) {
            thl->logf(thl, 0, LOG_LEVELS_ERROR,
                      "failed to start listening on udp socket with error %s",
                      strerror(errno));
            goto EXIT;
        }

        thl->log(thl, 0, "accepting udp connections", LOG_LEVELS_INFO);
    }

    socket_server_t *server =
        calloc(sizeof(socket_server_t), sizeof(socket_server_t));
    if (server == NULL) {
        thl->log(thl, 0, "failed to calloc socket server", LOG_LEVELS_ERROR);
        goto EXIT;
    }

    server->udp_socket_number = udp_socket_num;
    server->tcp_socket_number = tcp_socket_num;
    server->thl = thl;
    server->thl->log(server->thl, 0, "initialized server", LOG_LEVELS_INFO);

    freeaddrinfo(tcp_bind_address);
    freeaddrinfo(udp_bind_address);

    return server;

EXIT:

    if (tcp_bind_address != NULL) {
        freeaddrinfo(tcp_bind_address);
    }

    if (udp_bind_address != NULL) {
        freeaddrinfo(udp_bind_address);
    }

    if (tcp_socket_num != 0) {
        close(tcp_socket_num);
    }

    if (udp_socket_num != 0) {
        close(udp_socket_num);
    }

    return NULL;
}

/*! @brief terminates a server and frees up resources associated with it
 */
void free_socket_server(socket_server_t *srv) {
    if (srv->tcp_socket_number > 0) {
        close(srv->tcp_socket_number);
        srv->thl->log(srv->thl, 0, "shut down udp socket", LOG_LEVELS_INFO);
    }
    if (srv->udp_socket_number > 0) {
        close(srv->udp_socket_number);
        srv->thl->log(srv->thl, 0, "shut down udp socket", LOG_LEVELS_INFO);
    }
    pthread_join(srv->thread, NULL);
    srv->thl->log(srv->thl, 0, "server shutdown, goodbye", LOG_LEVELS_INFO);
    clear_thread_logger(srv->thl);
    free(srv);
    srv = NULL;
}