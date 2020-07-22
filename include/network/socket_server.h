/*! @file socket_server.h
 * @brief used to create a tcp/udp socket server listening on multiaddrs
 */

#pragma once

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

/*!
  * @brief defines a function signature that is used for submitting tasks to our thread pool
*/
typedef void (threadpool_task_func)(void *data);

typedef struct socket_server_config {
    char *udp_port_number;
    char *tcp_port_number;
    char *listen_address;
    int max_connections;
    int num_threads;
} socket_server_config_t;

typedef struct socket_server {
    int udp_socket_number;
    int tcp_socket_number;
    thread_logger *thl;
    pthread_t main_thread; /*! @note main thread responsible for processing all connections */
    threadpool thpool;
    threadpool_task_func *task_func;
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

/*! @brief returns a new socket server bound to the port number and ready to accept
 * connections
 */
socket_server_t *new_socket_server(thread_logger *thl,
                                   socket_server_config_t config);

/*! @brief listens for new connections and spawns a thread to process the connection
 * thread that is created to process the connection runs as a detached thread
 * will poll for new connections to accept every 500 miliseconds
 * @param data void pointer to a socket_server struct
 * @note detached thread created calling async_handle_conn_func
 * @warning may change the 500 milisecond sleep
 */
void *async_listen_func(void *data);

/*! @brief helper function for accepting client connections
 * times out new attempts if they take 3 seconds or more
 * @return Failure: NULL client conn failed
 * @return Success: non-NULL populated client_conn object
 */
client_conn_t *accept_client_conn(socket_server_t *srv);

/*! @brief terminates a server and frees up resources associated with it
 */
void free_socket_server(socket_server_t *srv);

/*!
  * @brief starts the socket server which processes new connections
  * @details when a new connection is accepted (tcp) OR we can receive data on a udp socket, the given handle_conn_func is used to process that client connection
  * @param srv an instance of a socket_server_t that has been initialized through new_socket_server
  * @param fn the main task to submit to our thread pool, it should be responsible for accepting new connections and procesing them
*/
void start_socket_socker(socket_server_t *srv, threadpool_task_func *fn);

/*!
  * @brief dedicated function for accepting and handling new connections
  * @note processes new connections by launching them 
*/
void *accept_connections(void *data);

/*!
 * @brief example function used to showcase how you can handle connections
 * @note in general should accept a conn_handle_data_t type but this is implementation define
*/
void example_task_func(void *data);