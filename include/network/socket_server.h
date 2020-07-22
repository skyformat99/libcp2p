/*! @file socket_server.h
 * @brief used to create a tcp/udp socket server listening on multiaddrs
 */

#pragma once

#include "multiaddr/multiaddr.h"
#include "network/socket_client.h" // this also imports socket.h
#include "utils/logger.h"
#include "utils/thread_pool.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>

#define MAX_ADDRS 100

/*!
 * @brief defines a function signature that is used for submitting tasks to our
 * thread pool
 */
typedef void(threadpool_task_func)(void *data);

/*!
 * @struct socket_server_config
 * @typedef socket_server_config_t
 * @brief used for configuring a socket_server_t instance
 * @todo switch to multiaddresses
 */
typedef struct socket_server_config {
    /*! @brief the thread pool task function to use for processing tcp connections */
    threadpool_task_func *fn_tcp;
    /*! @brief the thread pool task function to use for processing tcp connections */
    threadpool_task_func *fn_udp;
    int max_connections;
    int num_threads;
    int num_addrs;
    multi_addr_t **addrs;
} socket_server_config_t;

/*!
 * @struct socket_server
 * @typedef socket_server_t
 * @brief a TCP/UDP socket server
 * @details provides a thread pool can be used for handling new connections
 * @todo dont limit to two sockets (this will requiring using multiaddrs and an array
 * of socket numbers)
 */
typedef struct socket_server {
    int max_socket_num;
    fd_set grouped_socket_set;
    fd_set tcp_socket_set;
    fd_set udp_socket_set;
    thread_logger *thl;
    /*! @brief a thread pool that allows submitting worker tasks to a pool of threads
     */
    threadpool thpool;
    /*! @brief used for submitting a task to the thread pool for processing a tcp
     * connection */
    threadpool_task_func *task_func_tcp;
    /*! @brief used for submitting a task to the thread pool for processing a udp
     * connection */
    threadpool_task_func *task_func_udp;
} socket_server_t;

/*! @typedef client_conn
 * @struct client_conn
 * @brief a structure containing a file descriptor and address information
 * @todo
 *   - enable a queue/list of these
 */
typedef struct client_conn {
    int socket_number;
    sock_addr_storage *address;
} client_conn_t;

/*! @typedef conn_handle_data
 * @struct conn_handle_data
 * @brief struct containing arguments passed into pthread
 */
typedef struct conn_handle_data {
    socket_server_t *srv;
    client_conn_t *conn;
} conn_handle_data_t;

/*!
 * @brief used to create a TCP/UDP socket server ready to accept connections
 * @param thl an instance of a thread_logger
 * @param config the configuration settings used for the tcp/udp server
 * @return Success: pointer to a socket_server_t instance
 * @return Failure: NULL pointer
 */
socket_server_t *new_socket_server(thread_logger *thl,
                                   socket_server_config_t config);

/*! @brief helper function for accepting client connections
 * times out new attempts if they take 3 seconds or more
 * @return Failure: NULL client conn failed
 * @return Success: non-NULL populated client_conn object
 */
client_conn_t *accept_client_conn(socket_server_t *srv, int socket_num);

/*! @brief terminates a server and frees up resources associated with it
 */
void free_socket_server(socket_server_t *srv);

/*!
 * @brief starts the socket server which processes new connections
 * @details when a new connection is accepted (tcp) OR we can receive data on a udp
 * socket, the given handle_conn_func is used to process that client connection
 * @param srv an instance of a socket_server_t that has been initialized through
 * new_socket_server
 * @param fn_tcp
 * @param fn_udp
 */
void start_socket_server(socket_server_t *srv);

/*!
 * @brief dedicated function for accepting and handling new connections
 * @note processes new connections by launching them
 */
void *accept_connections(void *data);

/*!
 * @brief used to signal that we should exit the main start_socket_server function
 * @note this is only useful if you launch start_socket_server in a thread
 */
void signal_shutdown();

/*!
  * @brief used to free up resources allocated for socket_server_config_t
  * @param config an instance of socket_server_config_t initialized with new_socket_server_config 
*/
void free_socket_server_config(socket_server_config_t *config);

/*!
  * @brief used to initialize a socket_server_config_t object
  * @param num_addrs the number of multi_addr_t objects the addrs member will contain
  * @return Success: pointer to an initialized block of memory for socket_server_config_t
  * @return Failure: NULL pointer
*/
socket_server_config_t *new_socket_server_config();