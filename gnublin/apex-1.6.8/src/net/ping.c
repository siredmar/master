/* ping.c

   written by Marc Singer
   25 Jul 2005

   Copyright (C) 2005 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

   Implementation of ping command.

*/

#include <config.h>
#include <linux/string.h>
#include <linux/kernel.h>
#include <linux/types.h>
//#include <linux/ctype.h>
#include <apex.h>
#include <command.h>
#include <driver.h>
#include <error.h>
#include <alias.h>
#include <spinner.h>

#include <network.h>
#include <ethernet.h>

//#define TALK 2

#include <talk.h>

#define TRIES_MAX	(4)
#define MS_TIMEOUT	(1*1000)
#define PING_DATA_LENGTH (56)


/* ping_terminate

   is the function used by ethernet_service() to deterine when to
   terminate the loop.  It return zero when the loop can continue, -1
   on timeout, and 1 when the configuration is complete.

   *** FIXME: this is redundant

*/

static int ping_terminate (void* pv)
{
  struct ethernet_timeout_context* context
    = (struct ethernet_timeout_context*) pv;

  if (!context->time_start)
    context->time_start = timer_read ();

  return timer_delta (context->time_start, timer_read ()) < context->ms_timeout
    ? 0 : -1;
}


/* ping_receiver

   accepts ICMP packets from the ethernet receive layer.

*/

int ping_receiver (struct descriptor_d* d, struct ethernet_frame* frame,
		   void* context)
{
  int l;

  DBG (1, "%s\n", __FUNCTION__);

	/* Vet the frame */
  if (frame->cb < (sizeof (struct header_ethernet) + sizeof (struct header_arp)
	    + 6*2 + 4*2))
    return 0;			/* runt */
  if (   ETH_F (frame)->protocol != HTONS (ETH_PROTO_IP)
      || IPV4_F (frame)->protocol != IP_PROTO_ICMP)
    return 0;

  l = htons (IPV4_F (frame)->length) - sizeof (struct header_ipv4);
  DBG (2,"%s: checksum %x  calc %x  over %d\n", __FUNCTION__,
	  ICMP_F (frame)->checksum,
	  checksum (ICMP_F (frame), l), l);

  if (checksum (ICMP_F (frame), l) != 0) {
    DBG (1,"%s: icmp discarded, header checksum incorrect\n", __FUNCTION__);
    return -1;
  }

  if (ICMP_F (frame)->type == ICMP_TYPE_ECHO_REPLY) {
    printf ("ping seq %d received\n", htons (ICMP_PING_F (frame)->sequence));
    return 1;
  }

  return 0;
}


int cmd_ping (int argc, const char** argv)
{
  struct descriptor_d d;
  int result;
  struct ethernet_frame* frame;
  int tries = 0;
  char ip_address[4];
  const char* hardware_address;
  int cbData = PING_DATA_LENGTH;

  if (argc != 2)
    ERROR_RETURN (ERROR_PARAM, "target address required");

  if (UNCONFIGURED_IP)
    ERROR_RETURN (ERROR_FAILURE, "IP address not configured");

  result = getaddr (argv[1], ip_address);
//  printf ("result %d  %d.%d.%d.%d\n", result,
//	  ip_address[0], ip_address[1], ip_address[2], ip_address[3]);
  if (result)
    return result;

  if (   (result = parse_descriptor (szNetDriver, &d))
      || (result = open_descriptor (&d)))
    return result;

  hardware_address = arp_resolve (&d, ip_address, 0);

  if (!hardware_address)
    ERROR_RETURN (ERROR_PARAM, "no route to host");
//  printf ("hardware_address %02x:%02x:%02x:%02x:%02x:%02x\n",
//	  hardware_address[0], hardware_address[1], hardware_address[1],
//	  hardware_address[2], hardware_address[3], hardware_address[4]);

  frame = ethernet_frame_allocate ();

  memset (frame->rgb, 0, FRAME_LENGTH_MAX);
  memcpy (ETH_F (frame)->destination_address, hardware_address, 6);
  memcpy (ETH_F (frame)->source_address, host_mac_address, 6);
  ETH_F (frame)->protocol = HTONS (ETH_PROTO_IP);

  /* IPv4 header */
  IPV4_F (frame)->version_ihl = 4<<4 | 5;
  IPV4_F (frame)->length
    = htons (  sizeof (struct header_ipv4)
	     + sizeof (struct header_icmp)
	     + sizeof (struct message_icmp_ping)
	     + cbData);
  IPV4_F (frame)->ttl = 64;
  IPV4_F (frame)->protocol = IP_PROTO_ICMP;
  memcpy (IPV4_F (frame)->source_ip, host_ip_address, 4);
  memcpy (IPV4_F (frame)->destination_ip, ip_address, 4);
  IPV4_F (frame)->checksum
    = htons (checksum (IPV4_F (frame), sizeof (struct header_ipv4)));

  ICMP_F (frame)->type = ICMP_TYPE_ECHO;
//  ICMP_PING_F (frame)->identifier = 0xe5e5;
  memset (ICMP_PING_F (frame)->data, 0xa7, cbData);

  frame->cb = sizeof (struct header_ethernet)
	     + sizeof (struct header_ipv4)
	     + sizeof (struct header_icmp)
	     + sizeof (struct message_icmp_ping)
	     + cbData;

  register_ethernet_receiver (100, ping_receiver, NULL);

  do {
    struct ethernet_timeout_context timeout;

    ICMP_F (frame)->checksum = 0;
    ICMP_PING_F (frame)->sequence
      = htons (htons (ICMP_PING_F (frame)->sequence) + 1);
    ICMP_F (frame)->checksum
      = htons (checksum (ICMP_F (frame),
			 sizeof (struct header_icmp)
			 + sizeof (struct message_icmp_ping)
			 + cbData));

    DBG (1,"%s: send frame (%p %d)\n", __FUNCTION__, frame->rgb, frame->cb);
    d.driver->write (&d, frame->rgb, frame->cb);
    ++tries;

    memset (&timeout, 0, sizeof (timeout));
    timeout.ms_timeout = MS_TIMEOUT;
    result = ethernet_service (&d, ping_terminate, &timeout);
    /* result == 1 on success, -1 on timeout  */
  } while (result <= 0 && tries < TRIES_MAX);

  unregister_ethernet_receiver (ping_receiver, NULL);

  ethernet_frame_release (frame);

  close_descriptor (&d);

  return 0;
}

static __command struct command_d c_ping = {
  .command = "ping",
  .description = "ping command, ICMP echo request",
  .func = cmd_ping,
  COMMAND_HELP(
"ping ADDRESS\n"
"  Send ICMP echo requests to network host ADDRESS.\n"
  )
};
