/* CLUNC
 * Copyright 2009 Thomas Monjalon
 * Licensed under the GPLv3
 * See http://www.gnu.org/licenses/gpl.html
 *
 * Open a NetConsole in a LaCie U-Boot
 */

# include "net.h"
# include "lump.h"

# include <stdio.h>
# include <unistd.h>
# include <libgen.h>

# define eprintf(fmt, args...) fprintf (stderr, fmt "\n", ## args)

# define LUMP_PORT 4446
# define NETCONSOLE_PORT 6666

# define RETRY_DELAY 500 /* milliseconds */

static void print_usage (const char *cmd_name)
{
	eprintf ("usage: %s [-m MAC to target] [-i IP to set] [-v]", basename ((char*) cmd_name)) ;
}

static int wait_netconsole (int udp_socket, const struct lump *lump)
{
	ssize_t sent ;
	ssize_t received ;

	/* main loop (polling) */
	do {
		/* send LUMP */
		sent = send_broadcast (udp_socket, LUMP_PORT, PTR_SIZE (*lump)) ;
		if (sent <= 0)
			return -1 ;

		usleep (RETRY_DELAY * 1000) ;

		/* test if connected (should receive uboot prompt) */
		received = recv_test (udp_socket) ;
	} while (received == 0) ;

	return received <= 0 ;
}

static int catch_uboot (const char *target_mac, const char *new_ip) {
	int error_code = -1 ;
	int udp_socket = -1 ;
	struct lump lump ;

	/* open socket and bind to the NetConsole port */
	udp_socket = open_udp (NETCONSOLE_PORT) ;
	if (udp_socket < 0)
		goto end ;

	/* forge magic packet */
	error_code = write_lump (&lump, target_mac, NULL, new_ip) ;
	if (error_code != 0)
		goto end ;

	/* request and wait a connection */
	error_code = wait_netconsole (udp_socket, &lump) ;

end :
	if (udp_socket >= 0)
		close (udp_socket) ;
	return error_code ;
}

int main (int argc, char *argv[]) {
	int error_code = -1 ;
	int option ;
	char *target_mac = NULL ;
	char *new_ip = NULL ;

	while ((option = getopt (argc, argv, "hm:i:")) >= 0) {
		switch (option) {
			case 'h' :
				print_usage (argv[0]) ;
				error_code = 0 ;
				goto end ;
			case 'm' :
				target_mac = optarg ;
				break ;
			case 'i' :
				new_ip = optarg ;
				break ;
			default :
				print_usage (argv[0]) ;
				goto end ;
		}
	}
	if (argc > optind) {
		print_usage (argv[0]) ;
		goto end ;
	}

	error_code = catch_uboot (target_mac, new_ip) ;

end :
	return error_code == 0 ? EXIT_SUCCESS : EXIT_FAILURE ;
}
