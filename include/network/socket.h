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

/*! @file socket.h
 * @brief general socket related tooling
 */

#pragma once

#include "multiaddr/multiaddr.h"
#include "thirdparty/logger/logger.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>

#define MAX_RPC_MSG_SIZE_KB 8192

/*!
 * @brief internal boolean variable used to signal async start_socket_server function
 * calls
 */
extern bool do_shutdown;

/*!
 * @struct timeval
 * @typedef timeout
 * @brief used to specify timeouts for function calls such as select
 */
typedef struct timeval timeout;

/*! @typedef addr_info
 * @struct addrinfo
 * @brief alias for `struct addrinfo`
 */
typedef struct addrinfo addr_info;

/*! @typedef sock_addr
 * @struct sockaddr
 * @brief alias for `struct sockaddr`
 */
typedef struct sockaddr sock_addr;

/*! @typedef sock_addr_storage
 * @struct sockaddr_storage
 * @brief alias for `struct sockaddr_storage`
 */
typedef struct sockaddr_storage sock_addr_storage;

/*! @enum SOCKET_OPTS
 * @brief used to configure new sockets
 */
typedef enum {
    /*! sets socket with SO_REUSEADDR */
    REUSEADDR,
    /*! sets socket to non-blocking mode */
    NOBLOCK,
    /*! sets socket to blocking mode */
    BLOCK,
} SOCKET_OPTS;

/*! @brief  gets an available socket attached to bind_address
 * @return Success: file descriptor socket number greater than 0
 * @return Failure: -1
 * initializers a socket attached to bind_address with sock_opts, and binds the
 * address
 */
int get_new_socket(thread_logger *thl, addr_info *bind_address,
                   SOCKET_OPTS sock_opts[], int num_opts, bool is_client,
                   bool is_tcp);

/*! @brief used to enable/disable blocking sockets
 * @return Failure: false
 * @return Success: true
 * @note see
 * https://stackoverflow.com/questions/1543466/how-do-i-change-a-tcp-socket-to-be-non-blocking/1549344#1549344
 */
bool set_socket_blocking_status(int fd, bool blocking);

/*!
 * @brief sets a socket recv timeout
 * @param fd the file descriptor of the socket to apply operations to
 * @param seconds the seconds to timeout a recv or recvfrom after
 * @warning how does this workon UDP socket
 */
int set_socket_recv_timeout(int fd, int seconds);

/*!
 * @note this only works with tcp
 * @todo enable udp
 */
char *get_name_info(sock_addr *client_address);

/*! @brief generates an addr_info struct with defaults
 * defaults is IPv4, TCP, and AI_PASSIVE flags
 */
addr_info default_hints();

/*!
 * @brief used to check if a receive or send with a socket failed
 */
bool recv_or_send_failed(thread_logger *thl, int rc);

/*!
 * @brief returns an addr_info representation of the multiaddress
 * @details useful for taking a multi address and getting the needed information for
 * using
 * @details the address with the sendto function
 * @param address the multi address to parse
 * @note does not free up resources associated with address param
 * @warning only supports TCP and UDP multiaddress(es)
 * @return Success: pointer to an addr_info instance
 * @return Failure: NULL pointer
 */
addr_info *multi_addr_to_addr_info(multi_addr_t *address);