// Copyright 2020 RTrade Technologies Ltd
//
// licensed under GNU AFFERO GENERAL PUBLIC LICENSE;
// you may not use this file except in compliance with the License;
// You may obtain the license via the LICENSE file in the repository root;
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/*! @file socket_server.h
 * @brief used to create a tcp/udp socket server listening on multiaddrs
 */

#pragma once

#include "crypto/ecdsa.h"
#include "crypto/key.h"
#include "crypto/peerutils.h"
#include "multiaddr/multiaddr.h"
#include "network/messages.h"
#include "network/socket_client.h" // this also imports socket.h
#include "peerstore/peerstore.h"
#include "thirdparty/logger/logger.h"
#include "thirdparty/thread_pool/thread_pool.h"
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
 * @todo switch to multi_addresses
 * @details once you have used the config and created a new server with
 * new_socket_server() you can free the socket config with free_socket_config
 * @note once you have used the config and created a new server with
 * new_socket_server() you can free the socket config with free_socket_config
 */
typedef struct socket_server_config {
    int max_peers;
    int max_connections;
    int num_threads;
    int num_addrs;
    /*! @brief the timeout in seconds to set on a socket, 0 means nothing is set */
    int recv_timeout_sec;
    /*! @brief the thread pool task function to use for processing tcp connections */
    threadpool_task_func *fn_tcp;
    /*! @brief the addresses we will be listening on */
    multi_addr_t **addrs;
    /*! @brief the path to a pem encoded private ecdsa key */
    char *private_key_path;
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
    /*! @brief the maximum socket number 1 greater than our actual socket */
    int max_socket_num;
    /*! @brief contains all of our sockets */
    fd_set grouped_socket_set;
    /*! @brief contains our TCP sockets */
    fd_set tcp_socket_set;
    /*! @brief a thread pool that allows submitting worker tasks to a pool of threads
     */
    threadpool thpool;
    /*! @brief colored logging system */
    thread_logger *thl;
    /*! @brief used for submitting a task to the thread pool for processing a tcp
     * connection */
    threadpool_task_func *task_func_tcp;
    /*! @brief used for submitting a task to the thread pool for processing a udp
     * connection */
    threadpool_task_func *task_func_udp;
    /*! @brief used for storing information about our peers */
    peerstore_t *pstore;
    /*! @brief our ecdsa private key for handling cryptographic operations */
    ecdsa_private_key_t *private_key;
    /*! @brief our pubilc key information */
    public_key_t *public_key;
    /*! @brief our peer id information */
    peer_id_t *peer_id;
} socket_server_t;

/*! @typedef client_conn
 * @struct client_conn
 * @brief a structure containing a file descriptor and address information
 * @todo
 *   - enable a queue/list of these
 */
typedef struct client_conn {
    int socket_number;
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
 * @param sock_opts an array of options to configure the sockets we open with
 * @param num_opts the number of socket options we are using, providing a number that
 * does not match the actual number of options is undefined behavior
 * @return Success: pointer to a socket_server_t instance
 * @return Failure: NULL pointer
 * @details once you have used the config and created a new server with
 * @note once you have used the config and created a new server with
 */
socket_server_t *new_socket_server(thread_logger *thl,
                                   socket_server_config_t *config,
                                   SOCKET_OPTS sock_opts[], int num_opts);

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
 * @param config an instance of socket_server_config_t initialized with
 * new_socket_server_config
 */
void free_socket_server_config(socket_server_config_t *config);

/*!
 * @brief used to initialize a socket_server_config_t object
 * @param num_addrs the number of multi_addr_t objects the addrs member will contain
 * @return Success: pointer to an initialized block of memory for
 * socket_server_config_t
 * @return Failure: NULL pointer
 */
socket_server_config_t *new_socket_server_config(int num_addrs);

/*!
 * @brief handles receiving an rpc message from another peer
 * @note if you send an inbound message of `5hello` you'll invoke a debug handler to
 * print to stdout
 */
void handle_inbound_rpc(void *data);

/*!
 * @brief used to specify which syscall signals should trigger shutdown process
 */
void setup_signal_shutdown(int signals[], int num_signals);

/*!
 * @brief used to negotiate a secure connection with the current connection
 */
bool negotiate_secure_connection(conn_handle_data_t *data);

/*!
 * @brief handles receiving a hello protocol message from another peer
 * @details is responsible for exchanging identification information with a peer
 * @details and updating our peerstore with the appropriate information
 */
bool handle_hello_protocol(conn_handle_data_t *data, message_t *msg);

/*!
 * @brief used for a server to send a message to another server
 * @details this is a sort of "bi-directional RPC method" whereby a server can send a
 * @details request to another server acting as a client, but enabling either us
 * @details or the peer to invoke RPC methods. Essentially it is like
 * handle_inbound_rpc
 * @details except it is responsible for sending requests to a remote server, and any
 * responses
 * @details from the server are ran through handle_inbound_rpc
 * @return Success: 0
 * @return Failure: -1
 */
int socket_server_send(socket_server_t *srv, multi_addr_t *to_address,
                       message_t *msg);

/*!
 * @brief helper function to return a message_hello_t using our server values
 */
message_hello_t *new_server_message_hello_t(socket_server_t *srv);