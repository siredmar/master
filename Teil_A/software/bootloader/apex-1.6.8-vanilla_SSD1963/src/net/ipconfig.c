/* ipconfig.c

   written by Marc Singer
   1 Aug 2005

   Copyright (C) 2005 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

   IP configuration, either manual or by network protocol.

*/

#include <config.h>
#include <linux/string.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <apex.h>
#include <command.h>
#include <driver.h>
#include <error.h>
#include <alias.h>
#include <spinner.h>
#include <console.h>

#include <network.h>
#include <ethernet.h>

//#define TALK 2

#if defined (TALK) && TALK > 0
# define DBG(l,f...)		if (l <= TALK) printf (f);
#else
# define DBG(l,f...)		do {} while (0)
#endif


extern char host_ip_address[];
extern char server_ip_address[];
extern char gw_ip_address[];
extern char host_mac_address[];

#define TRIES_MAX	4
#define MS_TIMEOUT	(1*1000)


static void set_aliases (void)
{
#if defined (CONFIG_CMD_ALIAS)
  char sz[80];
  sprintf (sz, "%d.%d.%d.%d",
	   host_ip_address[0], host_ip_address[1],
	   host_ip_address[2], host_ip_address[3]);
  alias_set ("hostip", sz);
  sprintf (sz, "%d.%d.%d.%d",
	   server_ip_address[0], server_ip_address[1],
	   server_ip_address[2], server_ip_address[3]);
  alias_set ("serverip", sz);
  sprintf (sz, "%d.%d.%d.%d",
	   gw_ip_address[0], gw_ip_address[1],
	   gw_ip_address[2], gw_ip_address[3]);
  alias_set ("gatewayip", sz);
#endif
}

static void show_ip_config (void)
{
  if (UNCONFIGURED_IP)
    printf ("IP unconfigured\n");
  else {
    printf ("hostip %d.%d.%d.%d\n",
	    host_ip_address[0], host_ip_address[1],
	    host_ip_address[2], host_ip_address[3]);
    if (server_ip_address[0])
      printf ("serverip %d.%d.%d.%d\n",
	      server_ip_address[0], server_ip_address[1],
	      server_ip_address[2], server_ip_address[3]);
    if (gw_ip_address[0])
      printf ("gatewayip %d.%d.%d.%d\n",
	      gw_ip_address[0], gw_ip_address[1],
	      gw_ip_address[2], gw_ip_address[3]);
  }
}


/* ipconfig_terminate

   is the function used by ethernet_service() to deterine when to
   terminate the loop.  It return zero when the loop can continue, -1
   on timeout, and 1 when the configuration is complete.  -2 on user
   abort.

*/

static int ipconfig_terminate (void* pv)
{
  struct ethernet_timeout_context* context
    = (struct ethernet_timeout_context*) pv;

  if (!context->time_start)
    context->time_start = timer_read ();

  SPINNER_STEP;

  if (!UNCONFIGURED_IP)
    return 1;

  if (console->poll (0, 0))
    return ERROR_BREAK;

  return timer_delta (context->time_start, timer_read ()) < context->ms_timeout
    ? 0 : ERROR_TIMEOUT;
}


#if defined CONFIG_CMD_IPCONFIG_RARP

/* rarp_receiver

   accepts RARP packets from the ethernet receive layer.

*/

static int rarp_receiver (struct descriptor_d* d,
			  struct ethernet_frame* frame,
			  void* context)
{
#if 0
  printf ("%s len %d %d proto %x %x (%d %d)\n", __FUNCTION__, frame->cb,
	  sizeof (struct header_ethernet) + sizeof (struct header_arp),
	  ETH_F (frame)->protocol,
	  HTONS (ETH_PROTO_RARP),
	  ARP_F (frame)->hardware_address_length,
	  ARP_F (frame)->protocol_address_length);
#endif

	/* Vet the frame */
  if (frame->cb
      < (sizeof (struct header_ethernet) + sizeof (struct header_arp)))
    return 0;			/* runt */

  if (ETH_F (frame)->protocol != HTONS (ETH_PROTO_RARP))
    return 0;

  if (   ARP_F (frame)->hardware_address_length != 6
      || ARP_F (frame)->protocol_address_length != 4)
    return -1;			/* unrecognized form, discard */

  DBG (2,"%s: opcode %d \n", __FUNCTION__, HTONS (ARP_F (frame)->opcode));

  switch (ARP_F (frame)->opcode) {

  case HTONS (ARP_REVERSEREPLY):
    if (memcmp (ARP_F (frame)->target_hardware_address,
		host_mac_address, 6))
      break;
    memcpy (host_ip_address, ARP_F (frame)->target_protocol_address, 4);
		/* Add ARP entry for the server */
    arp_cache_update ((char*) ARP_F (frame)->sender_hardware_address,
		      (char*) ARP_F (frame)->sender_protocol_address,
		      1);
    memcpy (server_ip_address, ARP_F (frame)->sender_protocol_address, 4);
    memcpy (gw_ip_address, ARP_F (frame)->sender_protocol_address, 4);
    set_aliases ();
    break;
  }

  return 1;
}


int cmd_ipconfig_rarp (int argc, const char** argv)
{
  struct descriptor_d d;
  int result;
  struct ethernet_frame* frame;
  int tries = 0;

  if (   (result = parse_descriptor (szNetDriver, &d))
      || (result = open_descriptor (&d)))
    return result;

  DBG (2,"%s: open %s -> %d\n", __FUNCTION__, szNetDriver, result);

  frame = ethernet_frame_allocate ();

  DBG (2,"%s: setup ethernet header %p\n", __FUNCTION__, frame);

  memset (ETH_F (frame)->destination_address, 0xff, 6);
  memcpy (ETH_F (frame)->source_address, host_mac_address, 6);
  ETH_F (frame)->protocol = HTONS (ETH_PROTO_RARP);

  DBG (2,"%s: setup rarp header\n", __FUNCTION__);

  ARP_F (frame)->hardware_type = HTONS (ARP_HARDW_ETHERNET);
  ARP_F (frame)->protocol_type = HTONS (ARP_PROTO_IP);
  ARP_F (frame)->hardware_address_length = 6;
  ARP_F (frame)->protocol_address_length = 4;
  ARP_F (frame)->opcode = HTONS (ARP_REVERSEREQUEST);

  DBG (2,"%s: setup rarp\n", __FUNCTION__);

  memcpy (ARP_F (frame)->sender_hardware_address, host_mac_address, 6);
  memset (ARP_F (frame)->sender_protocol_address, 0, 4);
  memcpy (ARP_F (frame)->target_hardware_address,
	  ARP_F (frame)->sender_hardware_address, 10);
  frame->cb = sizeof (struct header_ethernet) + sizeof (struct header_arp);
//  dump (frame->rgb, frame->cb, 0);

  register_ethernet_receiver (100, rarp_receiver, NULL);

  goto flush;		/* Receive pending packets before first transmit  */
  do {
    struct ethernet_timeout_context timeout;

    DBG (1,"%s: send frame\n", __FUNCTION__);
    d.driver->write (&d, frame->rgb,
		     sizeof (struct header_ethernet)
		     + sizeof (struct header_arp));
    ++tries;

  flush:
    memset (&timeout, 0, sizeof (timeout));
    timeout.ms_timeout = MS_TIMEOUT;
    result = ethernet_service (&d, ipconfig_terminate, &timeout);

    /* result == 1 on success, -1 on timeout, -2 on user abort  */
  } while (result != ERROR_BREAK && result <= 0 && tries < TRIES_MAX);

  unregister_ethernet_receiver (rarp_receiver, NULL);

  printf ("\r");
  if (UNCONFIGURED_IP)
    printf ("RARP failed\n");
  else
    show_ip_config ();

  ethernet_frame_release (frame);

  close_descriptor (&d);

  return result < 0 ? result : 0;
}

#endif

#if defined CONFIG_CMD_IPCONFIG_BOOTP

static u16 _checksum (u32* sum, void* pv, int cb)
{
  u16* p = (u16*) pv;
  for (; cb > 0; cb -= 2) {
    unsigned short s = *p++;
    *sum += HTONS (s);
  }

  return ~ ((*sum & 0xffff) + (*sum >> 16));
}

static int bootp_receiver (struct descriptor_d* d,
			  struct ethernet_frame* frame,
			  void* context)
{
  /* Vet the frame */
  if (frame->cb
      < (sizeof (struct header_ethernet) + sizeof (struct header_ipv4)
	 + sizeof (struct header_udp) + sizeof (struct header_bootp)))
    return 0;			/* runt */

  if (ETH_F (frame)->protocol != HTONS (ETH_PROTO_IP))
    return 0;

  if (IPV4_F (frame)->protocol != IP_PROTO_UDP)
    return 0;

  if (UDP_F (frame)->source_port != HTONS (PORT_BOOTP_SERVER))
    return 0;

  if (UDP_F (frame)->destination_port != HTONS (PORT_BOOTP_CLIENT))
    return 0;

  if (BOOTP_F (frame)->op != 0x02)
    return 0;

  memcpy (host_ip_address, BOOTP_F (frame)->yiaddr, 4);
	/* Add ARP entry for the server */
  arp_cache_update ((char*) ETH_F (frame)->source_address,
		    (char*) IPV4_F (frame)->source_ip,
		    1);
  memcpy (server_ip_address, BOOTP_F (frame)->siaddr, 4);
  memcpy (gw_ip_address, BOOTP_F (frame)->giaddr, 4);
  set_aliases ();

  return 1;
}

int cmd_ipconfig_bootp (int argc, const char** argv)
{
  struct descriptor_d d;
  int result;
  struct ethernet_frame* frame;
  int tries = 0;

  if (   (result = parse_descriptor (szNetDriver, &d))
      || (result = open_descriptor (&d)))
    return result;

  DBG (2,"%s: open %s -> %d\n", __FUNCTION__, szNetDriver, result);

  frame = ethernet_frame_allocate ();

  DBG (2,"%s: setup ethernet header %p\n", __FUNCTION__, frame);

  memset (IPV4_F (frame), 0,
	  sizeof (struct header_ipv4) + sizeof (struct header_udp));
  
  memset (ETH_F (frame)->destination_address, 0xff, 6);
  memcpy (ETH_F (frame)->source_address, host_mac_address, 6);
  ETH_F (frame)->protocol = HTONS (ETH_PROTO_IP);

  DBG (2,"%s: setup ipv4 header\n", __FUNCTION__);

  IPV4_F (frame)->version_ihl = 4<<4 | 5;
  IPV4_F (frame)->length
    = htons (  sizeof (struct header_ipv4)
	     + sizeof (struct header_udp)
	     + sizeof (struct header_bootp));
  IPV4_F (frame)->ttl = 64;
  IPV4_F (frame)->protocol = IP_PROTO_UDP;
  memset (IPV4_F (frame)->source_ip, 0xff, 4);
  memset (IPV4_F (frame)->destination_ip, 0xff, 4);
  IPV4_F (frame)->checksum = 0;

  DBG (2,"%s: setup bootp header\n", __FUNCTION__);

  BOOTP_F (frame)->op = 0x01;
  BOOTP_F (frame)->htype = 0x01;
  BOOTP_F (frame)->hlen = 0x06;
  BOOTP_F (frame)->hops = 0x00;
  BOOTP_F (frame)->xid = HTONS (0x3903F326);
  BOOTP_F (frame)->secs = HTONS (0x0000);
  BOOTP_F (frame)->flags = HTONS (0x0000);
  memset (BOOTP_F (frame)->ciaddr, 0x00, 4);
  memset (BOOTP_F (frame)->yiaddr, 0x00, 4);
  memset (BOOTP_F (frame)->siaddr, 0x00, 4);
  memset (BOOTP_F (frame)->giaddr, 0x00, 4);
  memset (BOOTP_F (frame)->chaddr, 0x00, 16);
  memcpy (BOOTP_F (frame)->chaddr, host_mac_address, 6);
  memset (BOOTP_F (frame)->sname, 0x00, 64);
  memset (BOOTP_F (frame)->file, 0x00, 128);

  /* This must be at the end because the UDP checksum includs the data */
  DBG (2,"%s: setup udp header\n", __FUNCTION__);

  UDP_F (frame)->source_port = HTONS (PORT_BOOTP_CLIENT);
  UDP_F (frame)->destination_port = HTONS (PORT_BOOTP_SERVER);
  UDP_F (frame)->length = htons (sizeof (struct header_udp) + sizeof (struct header_bootp));
  {
    /* UDP Checksum use part of IPv4 header*/
    u32 sum = 0;
    u16 length = htons (sizeof (struct header_udp) + sizeof (struct header_bootp));
    u8 rgb[2] = { 0, IPV4_F (frame)->protocol };
    _checksum (&sum, IPV4_F (frame)->source_ip, 8);
    _checksum (&sum, rgb, 2);
    _checksum (&sum, &length, 2);
    UDP_F (frame)->checksum
      = htons (_checksum (&sum, UDP_F (frame),
			  sizeof (struct header_udp) + sizeof (struct header_bootp)));
  }

  IPV4_F (frame)->checksum
    = htons (checksum (IPV4_F (frame), sizeof (struct header_ipv4)));

  frame->cb = sizeof (struct header_ethernet)
	    + sizeof (struct header_ipv4)
	    + sizeof (struct header_udp)
	    + sizeof (struct header_bootp);
//  dump (frame->rgb, frame->cb, 0);

  register_ethernet_receiver (100, bootp_receiver, NULL);

  goto flush;		/* Receive pending packets before first transmit  */
  do {
    struct ethernet_timeout_context timeout;

    DBG (1,"%s: send frame\n", __FUNCTION__);
    d.driver->write (&d, frame->rgb,
		     sizeof (struct header_ethernet)
		     + sizeof (struct header_ipv4)
		     + sizeof (struct header_udp)
		     + sizeof (struct header_bootp));
    ++tries;

  flush:
    memset (&timeout, 0, sizeof (timeout));
    timeout.ms_timeout = MS_TIMEOUT;
    result = ethernet_service (&d, ipconfig_terminate, &timeout);

    /* result == 1 on success, -1 on timeout, -2 on user abort  */
  } while (result != ERROR_BREAK && result <= 0 && tries < TRIES_MAX);

  unregister_ethernet_receiver (bootp_receiver, NULL);

  printf ("\r");
  if (UNCONFIGURED_IP)
    printf ("BOOTP failed\n");
  else
    show_ip_config ();

  ethernet_frame_release (frame);

  close_descriptor (&d);

  return result < 0 ? result : 0;
}

#endif

int cmd_ipconfig (int argc, const char** argv)
{
  int result;

  if (argc == 1) {
    goto show;
  alreadyconfig:
    printf ("IP already configured\n");
  show:
    show_ip_config ();
    return 0;
  }

  if (argc >= 2) {
    if (PARTIAL_MATCH (argv[1], "c", "lear") == 0) {
      memset (host_ip_address, 0, 4);
      memset (server_ip_address, 0, 4);
      memset (gw_ip_address, 0, 4);
#if defined (CONFIG_CMD_ALIAS)
      alias_unset ("hostip");
      alias_unset ("serverip");
      alias_unset ("gatewayip");
#endif
      goto done;
    }

#if defined (CONFIG_CMD_IPCONFIG_RARP)
    if (PARTIAL_MATCH (argv[1], "r", "arp") == 0) {
      if (!UNCONFIGURED_IP)
	goto alreadyconfig;
      return cmd_ipconfig_rarp (argc, argv);
    }
#endif
#if defined (CONFIG_CMD_IPCONFIG_BOOTP)
    if (PARTIAL_MATCH (argv[1], "b", "ootp") == 0) {
      if (!UNCONFIGURED_IP)
	goto alreadyconfig;
      return cmd_ipconfig_bootp (argc, argv);
    }
#endif
#if defined (CONFIG_CMD_IPCONFIG_DHCP)
    if (PARTIAL_MATCH (argv[1], "d", "hcp") == 0) {
      if (!UNCONFIGURED_IP)
	goto alreadyconfig;
      return cmd_ipconfig_dhcp (argc, argv);
    }
#endif

#if defined (CONFIG_CMD_IPCONFIG_STATIC)
    result = getaddr (argv[1], host_ip_address);
    if (!result) {
      set_aliases ();
      goto show;
    }
#endif

    ERROR_RETURN (ERROR_PARAM, "unrecognized sub-command");
  }

 done:
  return 0;
}


static __command struct command_d c_ipconfig = {
  .command = "ipconfig",
  .description = "IP configuration",
  .func = cmd_ipconfig,
  COMMAND_HELP(
"ipconfig OPTIONS\n"
"  Configure the host IP address.  With no arguments, it shows the\n"
"  current IP configuration.\n"
"    ipconfig clear\n"
"      Reset the IP configuration.\n"
#if defined CONFIG_CMD_IPCONFIG_STATIC
"    ipconfig IPADDRESS\n"
//"    ipconfig IPADDRESS/PREFIX GATEWAY\n"
"      Manually configure the IP address.\n"
#endif
#if defined CONFIG_CMD_IPCONFIG_RARP
"    ipconfig rarp\n"
"      Configure using the RARP protocol.\n"
#endif
#if defined CONFIG_CMD_IPCONFIG_BOOTP
"    ipconfig bootp\n"
"      Configure using the BOOTP protocol.\n"
#endif
#if defined CONFIG_CMD_IPCONFIG_DHCP
"    ipconfig dhcp\n"
"      Configure using the DHCP protocol.\n"
#endif
  )
};
