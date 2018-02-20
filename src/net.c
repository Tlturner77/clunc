/* CLUNC
 * Copyright 2009 Thomas Monjalon
 * Licensed under the GPLv3
 * See http://www.gnu.org/licenses/gpl.html
 *
 * Network utilities
 */

# include "net.h"

# include <string.h>
# include <stdio.h>
# include <unistd.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <errno.h>

static void set_ipv4_address (struct sockaddr *address, in_addr_t ip_address, uint16_t port)
{
	memset (address, 0, sizeof *address) ;
	address->sa_family = AF_INET ;
	((struct sockaddr_in *) address)->sin_addr.s_addr = ip_address ;
	((struct sockaddr_in *) address)->sin_port = htons (port) ;
}

static int enable_reuse_port (int ip_socket)
{
	int error_code ;
# ifdef SO_REUSEPORT
	int option_name = SO_REUSEPORT ;
# else
	int option_name = SO_REUSEADDR ;
# endif
	int option_value = 1 ;
	error_code = setsockopt (ip_socket, SOL_SOCKET, option_name, PTR_SIZE (option_value)) ;
	if (error_code != 0)
		perror ("cannot set socket option (reuse)") ;
	return error_code ;
}

static int enable_broadcast (int ip_socket)
{
	int error_code ;
	int option_name = SO_BROADCAST ;
	int option_value = 1 ;
	error_code = setsockopt (ip_socket, SOL_SOCKET, option_name, PTR_SIZE (option_value)) ;
	if (error_code != 0)
		perror ("cannot set socket option (broadcast)") ;
	return error_code ;
}

int open_udp (uint16_t port)
{
	int error_code = -1 ;
	int udp_socket = -1 ;
	struct sockaddr local_address ;

	udp_socket = socket (PF_INET, SOCK_DGRAM, 0) ;
	if (udp_socket < 0) {
		perror ("cannot open an UDP socket") ;
		goto end ;
	}

	error_code = enable_reuse_port (udp_socket) ;
	if (error_code != 0)
		goto end ;

	error_code = enable_broadcast (udp_socket) ;
	if (error_code != 0)
		goto end ;

	set_ipv4_address (&local_address, INADDR_ANY, port) ;
	error_code = bind (udp_socket, PTR_SIZE (local_address)) ;
	if (error_code != 0) {
		perror ("cannot bind to local port") ;
		goto end ;
	}

end :
	if (error_code != 0) {
		if (udp_socket >= 0)
			close (udp_socket) ;
		return -1 ;
	}
	return udp_socket ;
}

ssize_t send_broadcast (int udp_socket, uint16_t port, const void *message, size_t length)
{
	ssize_t sent ;
	struct sockaddr broadcast_address ;
	set_ipv4_address (&broadcast_address, INADDR_BROADCAST, port) ;
	sent = sendto (udp_socket, message, length, 0, PTR_SIZE (broadcast_address)) ;
	if (sent < 0)
		perror ("sendto failed") ;
	return sent ;
}

ssize_t recv_test (int s)
{
	ssize_t received = recv (s, &s, 1, MSG_PEEK | MSG_DONTWAIT) ;
	if (received < 0) {
		if (errno == EAGAIN)
			received = 0 ;
		else
			perror ("recv failed") ;
	}
	return received ;
}
