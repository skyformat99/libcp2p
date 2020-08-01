#include "network/socket.h"
#include "network/utils.h"
#include <netdb.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdint.h>
#include <sys/socket.h>
#include <unistd.h>

#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wunused-function"

// from
// https://stackoverflow.com/questions/9140409/transfer-integer-over-a-socket-in-c/9142150#9142150

/*!
 * @brief utility function to send an int through a socket
 * @details handles situations where we may send less than expected bytes
 */
int send_int_tcp(int num, int fd) {
    int32_t conv = htonl(num);
    char *data = (char *)&conv;
    int left = sizeof(conv);
    int rc;
    do {

        rc = send(fd, data, left, 0);
        if (rc < 0) {
            if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
                // use select() or epoll() to wait for the socket to be writable
                // again
            } else if (errno != EINTR) {
                return -1;
            }
        } else {
            data += rc;
            left -= rc;
        }

    } while (left > 0);
    return 0;
}

/*!
 * @brief utility function to read an int through a socket
 * @details handles situations where we may read less than expected bytes
 */
int receive_int_tcp(int *num, int fd) {
    int32_t ret;
    char *data = (char *)&ret;
    int left = sizeof(ret);
    int rc;
    do {
        rc = recv(fd, data, left, 0);
        if (rc <= 0) { /* instead of ret */
            if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
                // use select() or epoll() to wait for the socket to be readable
                // again
            } else if (errno != EINTR) {
                return -1;
            }
        } else {
            data += rc;
            left -= rc;
        }
    } while (left > 0);
    *num = ntohl(ret);
    return 0;
}

/*!
 * @brief utility function to send an int through a udp socket
 * @details handles situations where we may send less than expected bytes
 */
int send_int_udp(int num, int fd, addr_info *peer_address) {
    int32_t conv = htonl(num);
    char *data = (char *)&conv;
    int left = sizeof(conv);
    int rc;
    do {

        rc = sendto(fd, data, left, 0, peer_address->ai_addr, peer_address->ai_addrlen);
        if (rc < 0) {
            if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
                // use select() or epoll() to wait for the socket to be writable
                // again
            } else if (errno != EINTR) {
                return -1;
            }
        } else {
            data += rc;
            left -= rc;
        }

    } while (left > 0);
    return 0;
}

/*!
 * @brief utility function to read an int through a udp socket
 * @details handles situations where we may read less than expected bytes
 * @warning does not do anything with peer_address yet 
 */
int receive_int_udp(int *num, int fd, addr_info *peer_address) {
    int32_t ret;
    char *data = (char *)&ret;
    int left = sizeof(ret);
    int rc;
    do {
        rc = recv(fd, data, left, 0);
        if (rc <= 0) { /* instead of ret */
            if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
                // use select() or epoll() to wait for the socket to be readable
                // again
            } else if (errno != EINTR) {
                return -1;
            }
        } else {
            data += rc;
            left -= rc;
        }
    } while (left > 0);
    *num = ntohl(ret);
    return 0;
}