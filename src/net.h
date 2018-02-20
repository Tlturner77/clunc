/* CLUNC
 * Copyright 2009 Thomas Monjalon
 * Licensed under the GPLv3
 * See http://www.gnu.org/licenses/gpl.html
 *
 * Network utilities
 */

# ifndef NET_H
# define NET_H

# include <stdint.h>
# include <stdlib.h>
#include <unistd.h>

# define PTR_SIZE(x) &x, sizeof x

/* Open an UDP socket and bind to port */
int open_udp (uint16_t port) ;

/* Broadcast message to port via UDP socket */
ssize_t send_broadcast (int udp_socket, uint16_t port, const void *message, size_t length) ;

/* Test if something is received on socket s */
ssize_t recv_test (int s) ;

# endif /* NET_H */
