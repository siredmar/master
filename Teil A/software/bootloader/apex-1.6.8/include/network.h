/* network.h

   written by Marc Singer
   5 May 2005

   Copyright (C) 2005 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

   Declarations for APEXs super-simple, network stack.

*/

#if !defined (__NETWORK_H__)
#    define   __NETWORK_H__

/* ----- Includes */

/* ----- Types */

#include <linux/types.h>

#define NTOHS(v) ((((v) >> 8) & 0xff) | (((v) & 0xff) << 8))
#define HTONS(v) ((((v) >> 8) & 0xff) | (((v) & 0xff) << 8))

#define octetstoip(a,b,c,d)\
   ((a & 0xff) <<  0)\
 | ((b & 0xff) <<  8)\
 | ((c & 0xff) << 16)\
 | ((d & 0xff) << 24)

struct header_ethernet {
  u8 destination_address[6];
  u8 source_address[6];
  u16 protocol;
} __attribute__((packed));

struct header_arp {
  u16 hardware_type;
  u16 protocol_type;
  u8  hardware_address_length;
  u8  protocol_address_length;
  u16 opcode;
  /* These fields depend on the address lengths above */
  u8 sender_hardware_address[6];
  u8 sender_protocol_address[4];
  u8 target_hardware_address[6];
  u8 target_protocol_address[4];
} __attribute__((packed));

struct header_bootp {
  u8 op;
  u8 htype;
  u8 hlen;
  u8 hops;
  u32 xid;
  u16 secs;
  u16 flags;
  u8 ciaddr[4];
  u8 yiaddr[4];
  u8 siaddr[4];
  u8 giaddr[4];
  u8 chaddr[16];
  u8 sname[64];
  u8 file[128];
  u8 vend[64]; /* This field is not part of DHCP */
}  __attribute__((packed));

struct header_ipv4 {
  u8  version_ihl;		/* version (4 lsb) and header length (4 msb) */
  u8  tos;			/* type of service */
  u16 length;			/* packet length */
  u32 fragment;
  u8  ttl;
  u8  protocol;
  u16 checksum;
  u8  source_ip[4];
  u8  destination_ip[4];
} __attribute__((packed));

struct header_icmp {
  u8 type;
  u8 code;
  u16 checksum;
} __attribute__((packed));

struct message_icmp_ping {
  u16 identifier;
  u16 sequence;
  u8 data[];
} __attribute__((packed));

struct message_tftp {
  u16 opcode;
  u8 data[];
} __attribute__((packed));

struct header_udp {
  u16 source_port;
  u16 destination_port;
  u16 length;
  u16 checksum;
  u8 data[];
} __attribute__((packed));

#if 0
struct addrinfo {
  int ai_flags;
  int ai_family;
  int ai_socktype;
  int ai_protocol;
  size_t ai_addrlen;
  char* ai_canonname;
  struct sockaddr* ai_addr;
  struct addrinfo* ai_next;
};
#endif

/* ----- Globals */

/* ----- Prototypes */

	/* Generic PHY registers and bits */

#define PHY_CONTROL	0
#define PHY_STATUS	1
#define PHY_ID1		2
#define PHY_ID2		3

#define PHY_CONTROL_RESET		(1<<15)
#define PHY_CONTROL_LOOPBACK		(1<<14)
#define PHY_CONTROL_POWERDOWN		(1<<11)
#define PHY_CONTROL_ANEN_ENABLE		(1<<12)
#define PHY_CONTROL_RESTART_ANEN	(1<<9)
#define PHY_STATUS_ANEN_COMPLETE	(1<<5)
#define PHY_STATUS_LINK			(1<<2)
#define PHY_STATUS_100FULL		(1<<14)
#define PHY_STATUS_100HALF		(1<<13)
#define PHY_STATUS_10FULL		(1<<12)
#define PHY_STATUS_10HALF		(1<<11)

	/* Ethernet/IP protocols */

#define ETH_PROTO_IP		0x0800
#define ETH_PROTO_ARP		0x0806
#define ETH_PROTO_RARP		0x8035

#define IP_PROTO_ICMP		1
#define IP_PROTO_UDP		17

#define ARP_HARDW_ETHERNET	1
#define ARP_HARDW_IEEE802	6

#define ARP_PROTO_IP		0x0800

#define ARP_REQUEST		1
#define ARP_REPLY		2
#define ARP_REVERSEREQUEST	3
#define ARP_REVERSEREPLY	4
#define ARP_NAK			10

#define PORT_BOOTP_SERVER	67
#define PORT_BOOTP_CLIENT	68

#define ICMP_TYPE_ECHO		8
#define ICMP_TYPE_ECHO_REPLY	0

#define PORT_TFTP		69

#define TFTP_RRQ		1
#define TFTP_WRQ		2
#define TFTP_DATA		3
#define TFTP_ACK		4
#define TFTP_ERROR		5
#define TFTP_OACK		6

#define TFTP_ERROR_NONE		0
#define TFTP_ERROR_FILENOTFOUND	1
#define TFTP_ERROR_ACCESSERROR	2
#define TFTP_ERROR_DISKFULL	3
#define TFTP_ERROR_ILLEGALOP	4
#define TFTP_ERROR_UNKNOWNID	5
#define TFTP_ERROR_FILEEXISTS	6
#define TFTP_ERROR_NOUSER	7
#define TFTP_ERROR_OPTIONTERMINATE 8

#define ETH_F(f)	((struct header_ethernet*) (f->rgb))

#define ARP_F(f)	((struct header_arp*)\
			 (f->rgb\
			  + sizeof (struct header_ethernet)))

#define IPV4_F(f)	((struct header_ipv4*)\
			 (f->rgb\
			  + sizeof (struct header_ethernet)))

#define UDP_F(f)	((struct header_udp*)\
			 (f->rgb\
			  + sizeof (struct header_ethernet)\
			  + sizeof (struct header_ipv4)))

#define BOOTP_F(f)	((struct header_bootp*)\
			 (f->rgb\
			  + sizeof (struct header_ethernet)\
			  + sizeof (struct header_ipv4)\
			  + sizeof (struct header_udp)))

#define ICMP_F(f)	((struct header_icmp*)\
			 (f->rgb\
			  + sizeof (struct header_ethernet)\
			  + sizeof (struct header_ipv4)))

#define ICMP_PING_F(f)	((struct message_icmp_ping*)\
			 (f->rgb\
			  + sizeof (struct header_ethernet)\
			  + sizeof (struct header_ipv4)\
			  + sizeof (struct header_icmp)))

#define TFTP_F(f)	((struct message_tftp*)\
			 (f->rgb\
			  + sizeof (struct header_ethernet)\
			  + sizeof (struct header_ipv4)\
			  + sizeof (struct header_udp)))


extern char host_ip_address[];
extern char gw_ip_address[];
extern char host_mac_address[];
extern const char szNetDriver[];

#define UNCONFIGURED_IP ((*(u32*) host_ip_address) == 0)

#endif  /* __NETWORK_H__ */
