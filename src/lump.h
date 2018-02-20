/* CLUNC
 * Copyright 2009 Thomas Monjalon
 * Licensed under the GPLv3
 * See http://www.gnu.org/licenses/gpl.html
 *
 * LUMP (LaCie Updater Magic Packet)
 * The UDP packet is read by the customized U-Boot of LaCie.
 * Its role is to configure U-Boot and trigger NetConsole.
 */

# ifndef LUMP_H
# define LUMP_H

# include <stdint.h>
# include <netinet/in.h>
# include "ethernet.h"

# define LUMP_MAC_OFFSET 2

struct lump_header {
	char name [4] ;
	uint32_t size ;
} ;

# define LUMP_FIELD(type,value_size) \
	lump_ ## type ## _field { \
		struct lump_header field_header ; \
		struct lump_header type_header ; \
		uint8_t value [value_size] ; \
	}

struct LUMP_FIELD (generic, 0) ; /* lump_generic_field */
struct LUMP_FIELD (mac, LUMP_MAC_OFFSET + ETH_ALEN) ; /* lump_mac_field */
struct LUMP_FIELD (ip, sizeof (in_addr_t)) ; /* lump_ip_field */

struct lump {
	struct lump_header packet_header ;
	struct lump_mac_field target_mac ;
	struct lump_mac_field new_mac ;
	struct lump_ip_field new_ip ;
} ;

int write_lump (
	struct lump *lump,
	const char *target_mac,
	const char *new_mac,
	const char *new_ip
) ;

# endif /* LUMP_H */
