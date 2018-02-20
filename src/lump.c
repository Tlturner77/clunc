/* CLUNC
 * Copyright 2009 Thomas Monjalon
 * Licensed under the GPLv3
 * See http://www.gnu.org/licenses/gpl.html
 *
 * LUMP (LaCie Updater Magic Packet)
 */

# include "lump.h"

# include <stdlib.h>
# include <stdio.h>
# include <string.h>
# include <arpa/inet.h>
# include <errno.h>

# define eprintf(fmt, args...) fprintf (stderr, fmt "\n", ## args)

# define PTR_SIZE(x) (void *) &x, sizeof x

# define LUMP_PACKET "LUMP"
# define MAC_TYPE    "MAC@"
# define MACD_FIELD  "MACD"
# define MACS_FIELD  "MACS"
# define IP_TYPE     "IP@"
# define IPS_FIELD   "IPS"


static int convert_mac (uint8_t *bin, const char *str)
{
	return ether_aton (str, (struct ether_addr *) (bin + LUMP_MAC_OFFSET)) == NULL ;
}

static int convert_ip (uint8_t *bin, const char *str)
{
	return inet_pton (AF_INET, str, bin) <= 0 ;
}

static void write_header (struct lump_header *header, size_t size, const char *name)
{
	strncpy (header->name, name, sizeof header->name) ;
	header->size = htonl (size - sizeof *header) ;
}

static int write_field (
	struct lump_generic_field *field, size_t size,
	const char *field_name, const char *type_name,
	const char *value, int (*value_convert) (uint8_t*, const char*))
{
	int error_code = 0 ;

	write_header (&field->field_header, size, field_name) ;

	write_header (&field->type_header, size - sizeof field->field_header, type_name) ;

	if (value != NULL)
		error_code = value_convert (field->value, value) ;

	return error_code ;
}

int write_lump (
	struct lump *lump,
	const char *target_mac, const char *new_mac, const char *new_ip)
{
	int error_code ;

	memset (lump, 0, sizeof *lump) ;

	write_header (PTR_SIZE (*lump), LUMP_PACKET) ;

	error_code = write_field (PTR_SIZE (lump->target_mac), MACD_FIELD, MAC_TYPE, target_mac, convert_mac) ;
	if (error_code != 0) {
		eprintf ("syntax error (target MAC)") ;
		return -1 ;
	}

	error_code = write_field (PTR_SIZE (lump->new_mac), MACS_FIELD, MAC_TYPE, new_mac, convert_mac) ;
	if (error_code != 0) {
		eprintf ("syntax error (new MAC)") ;
		return -1 ;
	}

	error_code = write_field (PTR_SIZE (lump->new_ip), IPS_FIELD, IP_TYPE, new_ip, convert_ip) ;
	if (error_code != 0) {
		eprintf ("syntax error (new IP)") ;
		return -1 ;
	}

	return 0 ;
}
