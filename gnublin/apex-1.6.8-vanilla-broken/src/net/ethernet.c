/* ethernet.c

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

   NOTES
   -----

   o There is support for MAC address filtering that must be enabled
     by the ethernet driver.

   o We only save ARP mappings for addresses we've requested.  In
     other words, we need to request an address before the received
     ARP_REPLY packet will update the table.  This prevents the ARP
     cache from filling with the addresses of hosts we don't care
     about.

   o The multitude of byte-count compares and copies suggests that we
     be clever in how these are coded for optimal density.

   o The ICMP echo reply is not properly turned around.  Though it
     works, it does so because we send the reply to the same MAC
     address as the sender.  Really, we should be finding our route to
     the destination with ARP.  That said, it is probably OK to leave
     this as is.

   o UDP checksums.  There isn't a good reason to implement them as we
     can depend on ethernet's CRC checksum.  The truth is that the
     verification code didn't work the first time and I didn't want to
     keep working on it.

   o Casting.  The underlying structures use u8, u16 and u32, but the
     rest of APEX uses the simpler forms, char, short, and long.
     Understandably, this could cause problems with porting.  Instead
     of percolating these types up, I choose to cast to squash them.

*/

#include <config.h>
#include <linux/types.h>
#include <linux/ctype.h>
#include <linux/string.h>
#include <linux/kernel.h>
#include <apex.h>	/* printf */
#include <driver.h>
#include <service.h>
#include <network.h>
#include <ethernet.h>
#include <alias.h>
#include <sort.h>
#include <console.h>

//#define TALK 2
//#define DUMP_RECEIVERS


#if defined (TALK) && TALK > 0
# define DBG(l,f...)		if (l <= TALK) printf (f);
#else
# define DBG(l,f...)		do {} while (0)
#endif

#define ARP_TABLE_LENGTH	8
#define FRAME_TABLE_LENGTH	8

#define ARP_SECONDS_LIVE	30

struct arp_entry {
  u8  address[6];
  u8  ip[4];
  u32 seconds;		/* Number of seconds that this entry is valid */
};

char host_ip_address[4];
char server_ip_address[4];	/* Address of server that helped us along */
char gw_ip_address[4];
char netmask[4];		/* Required for gw routing */
char host_mac_address[6];
const char szNetDriver[] = "eth:";
static const char broadcast_mac_address[6] = { 0xff, 0xff, 0xff,
					       0xff, 0xff, 0xff };

#define ARP_TRIES_MAX	5
#define MS_ARP_TIMEOUT	1000

struct arp_terminate_context {
  const char* ip_address;
  struct ethernet_timeout_context timeout;
};

enum {
  state_free      = 0,
  state_allocated = 1,
  state_queued    = 2,
};

struct arp_entry arp_table[ARP_TABLE_LENGTH];
struct ethernet_frame frame_table[FRAME_TABLE_LENGTH];

struct ethernet_receiver {
  int priority;
  pfn_ethernet_receiver pfn;
  void* context;
};

#define MAX_RECEIVERS	32
static struct ethernet_receiver receivers[MAX_RECEIVERS];
static int cReceivers;		/* Number of receivers */

u16 _checksum (u32* sum, void* pv, int cb)
{
  u16* p = (u16*) pv;
  for (; cb > 0; cb -= 2) {
    unsigned short s = *p++;
    *sum += HTONS (s);
  }

  return ~ ((*sum & 0xffff) + (*sum >> 16));
}

u16 checksum (void* pv, int cb)
{
  u16* p = (u16*) pv;
  u32 sum = 0;
  for (; cb > 0; cb -= 2) {
    unsigned short s = *p++;
    sum += HTONS (s);
  }

  return ~ ((sum & 0xffff) + (sum >> 16));
}

u16 port_allocate (void)
{
  static u16 port;
  if (port == 0)
    return port = 23000;
  else
    return ++port;
}

/* ethernet_frame_allocate

*/

struct ethernet_frame* ethernet_frame_allocate (void)
{
  int i;
  for (i = 0; i < FRAME_TABLE_LENGTH; ++i)
    if (frame_table[i].state == state_free) {
      frame_table[i].state = state_allocated;
      DBG (1, "%s: %p\n", __FUNCTION__, &frame_table[i]);
      return &frame_table[i];
    }
  return NULL;
}

void ethernet_frame_release (struct ethernet_frame* frame)
{
  DBG (1, "%s: %p\n", __FUNCTION__, frame);
  frame->state = state_free;
}


/* getaddr

   returns a binary, four-byte IP address from a string representing
   the address.  This function is *not* compliant with POSIX or RFCs.
   It is intended to be a light-weight conversion function which is
   useful without being bloated.  Should we need domain name
   resolution or other services, this function should be replaced with
   something more robust.

   The caller is responsible for providing a null terminated address
   string as well as a destination buffer of sufficient length.

*/

int getaddr (const char* address, char* ip_address)
{
  int octet = 0;
  int len = 0;

  for (; len < 4; ++address) {
    if (isdigit (*address)) {
      octet = octet*10 + *address - '0';
      continue;
    }
    if (*address && *address != '.')
      return -1;
    if (octet > 255)
      return -1;
    *ip_address++ = octet;
    ++len;
    octet = 0;
    if (!*address)
      break;
  }

  if (*address)
    return -2;
  return len != 4 ? -3 : 0;
}


/* ethernet_frame_reply

   turns a frame around to reply to the sender's hardware address.

*/

void ethernet_frame_reply (struct ethernet_frame* f)
{
  memcpy (ETH_F (f)->destination_address, ETH_F (f)->source_address, 6);
  memcpy (ETH_F (f)->source_address,		     host_mac_address, 6);
}


void ipv4_frame_reply (struct ethernet_frame* f)
{
  memcpy (IPV4_F (f)->destination_ip, IPV4_F (f)->source_ip, 4);
  memcpy (IPV4_F (f)->source_ip, host_ip_address, 4);
  IPV4_F (f)->checksum = 0;
  IPV4_F (f)->checksum = htons (checksum ((void*) IPV4_F (f),
					  sizeof (struct header_ipv4)));
}


/* arp_cache_update

   accepts the data from ARP_REPLY packets and updates the ARP cache
   accordingly.  Note that we don't snoop ARP entries.  Instead, we
   only maintain ARP translations for hosts we're interested in.

*/

void arp_cache_update (const char* hardware_address,
		       const char* protocol_address,
		       int force)
{
  int i;
  int iEmpty = -1;
  for (i = 0; i < ARP_TABLE_LENGTH; ++i) {
    if (iEmpty == -1 && memcmp (arp_table[i].address, "\0\0\0\0\0", 6) == 0)
      iEmpty = i;
    if (memcmp (arp_table[i].ip, protocol_address, 4) == 0) {
      memcpy (arp_table[i].address,
	      hardware_address ? hardware_address : broadcast_mac_address, 6);
      arp_table[i].seconds = ARP_SECONDS_LIVE;
      return;
    }
  }
  if (force && iEmpty != -1) {
    memcpy (arp_table[iEmpty].address,
	    hardware_address ? hardware_address : broadcast_mac_address, 6);
    memcpy (arp_table[iEmpty].ip, protocol_address, 4);
  }
}


/* arp_cache_lookup

   returns a pointer to the MAC address for the given IP address or
   NULL if there is none.

*/

static const char* arp_cache_lookup (const char* protocol_address)
{
  int i;
  for (i = 0; i < ARP_TABLE_LENGTH; ++i)
    if (memcmp (arp_table[i].ip, protocol_address, 4) == 0) {
      if (memcmp (arp_table[i].address, broadcast_mac_address, 6) == 0)
	return NULL;
      return (char*) arp_table[i].address;
    }

  return NULL;
}


/* arp_receiver

   handles arp reply frames.  It performs two tasks.  It updates the
   ARP cache with interesting ARP reply frames.  It also responds to
   ARP requests.  The latter is necessary for interoperating with
   networked hosts.

*/

int arp_receiver (struct descriptor_d* d, struct ethernet_frame* frame,
		  void* context)
{
  DBG (1,"%s (%d)\n", __FUNCTION__, frame->cb);

  if (frame->cb
      < (sizeof (struct header_ethernet) + sizeof (struct header_arp)))
    return 0;			/* runt */

  DBG (2, "%s: proto %x\n", __FUNCTION__, HTONS (ETH_PROTO_ARP));
  if (ETH_F (frame)->protocol != HTONS (ETH_PROTO_ARP))
    return 0;

  if (   ARP_F (frame)->hardware_address_length != 6
      || ARP_F (frame)->protocol_address_length != 4)
    return -1;			/* unrecognized form */

  DBG (2,"%s: opcode %d \n", __FUNCTION__, HTONS (ARP_F (frame)->opcode));

  switch (ARP_F (frame)->opcode) {
  case HTONS (ARP_REQUEST):
    if (memcmp (host_ip_address, ARP_F (frame)->target_protocol_address, 4))
      return -1;		/* Not a match */

	/* Send reply to request for our address */
    ethernet_frame_reply (frame);
    ARP_F (frame)->opcode = HTONS (ARP_REPLY);

    memcpy (ARP_F (frame)->target_hardware_address,
	    ARP_F (frame)->sender_hardware_address,
	    10);		/* Move both the HW and protocol address */
    memcpy (ARP_F (frame)->sender_hardware_address,
	    host_mac_address,
	    6);
    memcpy (ARP_F (frame)->sender_protocol_address,
	    host_ip_address,
	    4);
    frame->cb = sizeof (struct header_ethernet) + sizeof (struct header_arp);
    d->driver->write (d, frame->rgb, frame->cb);
    break;

  case HTONS (ARP_REPLY):
    arp_cache_update ((char*) ARP_F (frame)->sender_hardware_address,
		      (char*) ARP_F (frame)->sender_protocol_address,
		      0);
    break;
  }

  return 0;
}

#if defined (CONFIG_PROTO_ICMP_ECHO)

int icmp_echo_receiver (struct descriptor_d* d, struct ethernet_frame* frame,
			void* context)
{
  int l;

  DBG (2,"%s\n", __FUNCTION__);

	/* Vet the frame */
  if (frame->cb < (sizeof (struct header_ethernet)
		   + sizeof (struct header_ipv4)
		   + sizeof (struct header_icmp)))
    return 0;			/* runt */
  if (   ETH_F (frame)->protocol != HTONS (ETH_PROTO_IP)
      || IPV4_F (frame)->protocol != IP_PROTO_ICMP)
    return 0;

  DBG (2,"%s: icmp %d received\n", __FUNCTION__, ICMP_F (frame)->type);

  l = htons (IPV4_F (frame)->length) - sizeof (struct header_ipv4);
  DBG (2,"%s: checksum %x  calc %x  over %d\n", __FUNCTION__,
	  ICMP_F (frame)->checksum,
	  checksum (ICMP_F (frame), l), l);

  if (checksum (ICMP_F (frame), l) != 0) {
    DBG (1,"%s: icmp discarded, header checksum incorrect\n", __FUNCTION__);
    return -1;
  }

  if (ICMP_F (frame)->type == ICMP_TYPE_ECHO) {
    ethernet_frame_reply (frame); /* This isn't really valid, is it? */
    ipv4_frame_reply (frame);
    ICMP_F (frame)->type = ICMP_TYPE_ECHO_REPLY;
    ICMP_F (frame)->checksum = 0;
    ICMP_F (frame)->checksum = htons (checksum (ICMP_F (frame), l));
    d->driver->write (d, frame->rgb, frame->cb);
    return 1;
  }
  return 0;
}

#endif


/* ethernet_receive

   accepts packets into the network stack.

*/

void ethernet_receive (struct descriptor_d* d, struct ethernet_frame* frame)
{
  DBG (1,"%s\n", __FUNCTION__);

  if (frame->cb < sizeof (struct header_ethernet))
    return;			/* runt */

  /* Check for a valid MAC address.  This is configurable since most
     drivers have exact match logic for MAC addresses.  Broadcast and
     exact matches get through.  Others will not be received.
     Alternatively, some drivers may use a hash table to determine
     when to receive a frame.  Such devices will require frame
     filtering by address. */
#if defined (CONFIG_MAC_FILTER)
  if (   memcmp (ETH_F (frame)->destination_address, host_mac_address, 6)
      && memcmp (ETH_F (frame)->destination_address, broadcast_mac_address, 6))
    return;			/* Not for us. */
#endif

  if (!UNCONFIGURED_IP) {
    /* Check for a valid IP address.  At present, this isn't strictly
       correct since we don't check for broadcast addresses.  Adding
       support for such shouldn't be difficult.  It just requires some
       bookkeeping. */
    if (ETH_F (frame)->protocol == HTONS (ETH_PROTO_IP)
        && memcmp (IPV4_F (frame)->destination_ip, host_ip_address, 4))
      return;			/* Not for us */
  }
	/* Invoke receivers */
  {
    int i;
    for (i = 0; i < cReceivers; ++i) {
      DBG (1, "%s: checking %p %d\n", __FUNCTION__, frame, frame->cb);
      if (   receivers[i].pfn
	  && receivers[i].pfn (d, frame, receivers[i].context))
	break;
    }
  }
}

void udp_setup (struct ethernet_frame* frame,
		const char* destination_ip, u16 destination_port,
		u16 source_port, size_t cb)
{
  const char* addr = arp_cache_lookup (destination_ip);
  size_t cbFrame;
//  if (!addr)
//    addr = arp_cache_lookup (gw_ip_address);
  if (!addr)			/* *** FIXME: shouldn't be possible */
    return;

//  cb = (cb + 1) & ~1;
  cbFrame = (cb + 1) & ~1;
  if (cbFrame != cb)
    *(UDP_F (frame)->data + cb) = 0; /* zero extra byte */

  memset (IPV4_F (frame), 0,
	  sizeof (struct header_ipv4) + sizeof (struct header_udp));

  /* Ethernet frame header */
  if (addr)
    memcpy (ETH_F (frame)->destination_address, addr, 6);
  else
    memcpy (ETH_F (frame)->destination_address, broadcast_mac_address, 6);
  memcpy (ETH_F (frame)->source_address, host_mac_address, 6);
  ETH_F (frame)->protocol = HTONS (ETH_PROTO_IP);

  /* IPv4 header */
  IPV4_F (frame)->version_ihl = 4<<4 | 5; /* Version 4; Header Length 5 */
  IPV4_F (frame)->length
    = htons (sizeof (struct header_ipv4) + sizeof (struct header_udp) + cb);
  IPV4_F (frame)->ttl = 64;
  IPV4_F (frame)->protocol = IP_PROTO_UDP;
  memcpy (IPV4_F (frame)->source_ip, host_ip_address, 4);
  memcpy (IPV4_F (frame)->destination_ip, destination_ip, 4);
  IPV4_F (frame)->checksum = 0;

  /* UDP header */
  UDP_F (frame)->source_port = HTONS (source_port);
  UDP_F (frame)->destination_port = HTONS (destination_port);
  UDP_F (frame)->length = htons (sizeof (struct header_udp) + cb);
  UDP_F (frame)->checksum = 0;

#if defined (CONFIG_UDP_CHECKSUM)

  /* Checksums, UDP and then TCP */
  {
    u32 sum = 0;
    u16 length = htons (sizeof (struct header_udp) + cb);
    u8 rgb[2] = { 0, IPV4_F (frame)->protocol };
    _checksum (&sum, IPV4_F (frame)->source_ip, 8);
    _checksum (&sum, rgb, 2);
    _checksum (&sum, &length, 2);
    UDP_F (frame)->checksum
      = htons (_checksum (&sum, UDP_F (frame),
			  sizeof (struct header_udp) + cb));
  }

#endif

  IPV4_F (frame)->checksum
    = htons (checksum (IPV4_F (frame), sizeof (struct header_ipv4)));

  frame->cb = sizeof (struct header_ethernet)
    + sizeof (struct header_ipv4)
    + sizeof (struct header_udp)
    + cbFrame;
}


#if defined (CONFIG_UDP_CHECKSUM)

int udp_checksum_verify (struct ethernet_frame* frame)
{
  size_t cb = frame->cb - sizeof (struct header_ethernet)
    + sizeof (struct header_ipv4)
    + sizeof (struct header_udp);
  u32 sum = 0;
  u16 length = htons (sizeof (struct header_udp) + cb);
  u8 rgb[2] = { 0, IPV4_F (frame)->protocol };

  if (UDP_F (frame)->checksum == 0)
      return 0;

  _checksum (&sum, IPV4_F (frame)->source_ip, 8);
  _checksum (&sum, rgb, 2);
  _checksum (&sum, &length, 2);
  printf ("cb %d  %x %x", cb,
	  UDP_F (frame)->checksum,
	  htons (_checksum (&sum, UDP_F (frame),
			    sizeof (struct header_udp) + cb)));
  return UDP_F (frame)->checksum
    != htons (_checksum (&sum, UDP_F (frame),
			 sizeof (struct header_udp) + cb));
}

#endif


/* ethernet_service

   is the template function for processing network traffic.  It reads
   packets on the ethernet device given and receives them until the
   termination function returns a non-zero result.  The context value
   passed to this function is passed along to the termination
   function.  The return value is the non-zero result from the
   termination function.

*/

int ethernet_service (struct descriptor_d* d,
		      int (*terminate) (void*), void* context)
{
  struct ethernet_frame* frame = ethernet_frame_allocate ();
  int result;

  do {
    frame->cb = d->driver->read (d, frame->rgb, FRAME_LENGTH_MAX);
    if (frame->cb > 0) {
      DBG (1, "%s: frame %p %d\n", __FUNCTION__, frame, frame->cb);
      ethernet_receive (d, frame);
      frame->cb = 0;
    }
    result = terminate (context);
  } while (result == 0);

  ethernet_frame_release (frame);

  return result;
}


/* ethernet_timeout

   is a termination function for ethernet_service that terminates
   after a timeout is exceeded.  The context pointer is a
   ethernet_timeout_context structure.  It may zeroed and just the
   timeout set, or the time_start may also be set.

*/

int ethernet_timeout (void* pv)
{
  struct ethernet_timeout_context* context
    = (struct ethernet_timeout_context*) pv;

  if (!context->time_start)
    context->time_start = timer_read ();

  return timer_delta (context->time_start, timer_read ()) < context->ms_timeout
    ? 0 : -1;
}

int compare_receivers (const void* _a, const void* _b)
{
  struct ethernet_receiver* a = (struct ethernet_receiver*) _a;
  struct ethernet_receiver* b = (struct ethernet_receiver*) _b;

  if (a->pfn == b->pfn)		/* Probably NULLs */
    return 0;

  if (a->pfn == NULL)
    return 1;
  if (b->pfn == NULL)
    return -1;

  if (a->priority != b->priority)
    return b->priority - a->priority;
  else
    return (unsigned long) a->pfn - (unsigned long) b->pfn;
}


#if defined (DUMP_RECEIVERS)
static void dump_receivers (void)
{
  int i;
  for (i = 0; i < cReceivers; ++i)
    printf ("receiver %d: %4d %p %p\n",
	    i, receivers[i].priority, receivers[i].pfn, receivers[i].context);
}
#endif

/* register_ethernet_receive

   adds a frame receiving function to the list of active receivers.
   The function pointer will be called with the received frame as well
   as the context pointer when a frame is received.  The priority
   value is used to sort the receivers.  The higher the priority, the
   earlier the receiver will appear in the list.

   It returns zero on success, non-zero if the receiver cannot be
   added to the active receiver list.

*/

int register_ethernet_receiver (int priority, pfn_ethernet_receiver pfn,
				void* context)
{
  if (cReceivers >= MAX_RECEIVERS)
    return -1;

  receivers[cReceivers].priority = priority;
  receivers[cReceivers].pfn	 = pfn;
  receivers[cReceivers].context	 = context;

  sort (receivers, ++cReceivers, sizeof (*receivers),
	compare_receivers, NULL);

#if defined (DUMP_RECEIVERS)
  dump_receivers ();
#endif

  return 0;
}


/* unregister_ethernet_receiver

   removes a frame receiver from the list of active receivers.  Note
   that it doesn't use the original priority when removing a receiver.

   It returns zero on success, non-zero if the requested receiver
   isn't found.

*/

int unregister_ethernet_receiver (pfn_ethernet_receiver pfn, void* context)
{
  int i;

  for (i = 0; i < cReceivers; ++i) {
    if (receivers[i].pfn == pfn && receivers[i].context == context) {
      memset (&receivers[i], 0, sizeof (receivers[i]));
      break;
    }
  }

  if (i < cReceivers) {
    sort (receivers, cReceivers--, sizeof (*receivers),
	  compare_receivers, NULL);
#if defined (DUMP_RECEIVERS)
    dump_receivers ();
#endif
    return 0;
  }

  return -1;
}

int arp_terminate (void* pv)
{
  struct arp_terminate_context* context = (struct arp_terminate_context*) pv;

  if (arp_cache_lookup (context->ip_address))
    return 1;

  if (console->poll (0, 0))
    return -2;			/* Not really a timeout */

  return ethernet_timeout (&context->timeout);
}


/* arp_resolve

   performs the essential protocol of querying the network for the
   hardware address associated with an ip address.  This is a
   self-contained protocol step that sends the arp request and waits
   for the response.  It will terminate if there is no resolution
   before the timeout.  It will return immediately if the mapping is
   present in the cache.

*/

const char* arp_resolve (struct descriptor_d* d, const char* ip_address,
			 int ms_timeout)
{
  const char* hardware_address = arp_cache_lookup (ip_address);
  struct ethernet_frame* frame;
  int tries = 0;
  int result;

  if (hardware_address)
    return hardware_address;

  frame = ethernet_frame_allocate ();
  memset (ETH_F (frame)->destination_address, 0xff, 6);
  memcpy (ETH_F (frame)->source_address, host_mac_address, 6);
  ETH_F (frame)->protocol = HTONS (ETH_PROTO_ARP);

  ARP_F (frame)->hardware_type = HTONS (ARP_HARDW_ETHERNET);
  ARP_F (frame)->protocol_type = HTONS (ARP_PROTO_IP);
  ARP_F (frame)->hardware_address_length = 6;
  ARP_F (frame)->protocol_address_length = 4;
  ARP_F (frame)->opcode = HTONS (ARP_REQUEST);

  memcpy (ARP_F (frame)->sender_hardware_address, host_mac_address, 6);
  memcpy (ARP_F (frame)->sender_protocol_address, host_ip_address, 4);
  memset (ARP_F (frame)->target_hardware_address, 0, 6);
  memcpy (ARP_F (frame)->target_protocol_address, ip_address, 4);
  frame->cb = sizeof (struct header_ethernet) + sizeof (struct header_arp);

  arp_cache_update (NULL, ip_address, 1);

  /* *** FIXME: need to check for the callers's timeout as well.  We
     should cascade this timeout through thea arp_terminate_context
     s.t. the user can perform the appropriate UI steps. */
  do {
    struct arp_terminate_context context;

    DBG (2, "transmitting arp request, %d bytes\n", frame->cb);
    DBG (2, "d %p  d->driver %p  d->driver->write %p\n",
	 d, d->driver, d->driver->write);

    d->driver->write (d, frame->rgb, frame->cb);
    ++tries;

    memset (&context, 0, sizeof (context));
    context.ip_address = ip_address;
    context.timeout.ms_timeout = MS_ARP_TIMEOUT;
    result = ethernet_service (d, arp_terminate, &context);
    /* result == 1 on success, -1 on timeout, -2 on cancel  */
  } while (result <= 0 && result != -2 && tries < ARP_TRIES_MAX);

  ethernet_frame_release (frame);

  return arp_cache_lookup (ip_address);
}


void ethernet_init (void)
{
#if defined (CONFIG_PROTO_ICMP_ECHO)
  register_ethernet_receiver (0, icmp_echo_receiver, NULL);
#endif
  register_ethernet_receiver (10, arp_receiver, NULL);
}

static __service_6 struct service_d ethernet_receiver_service = {
  .init = ethernet_init,
#if !defined (CONFIG_SMALL) && 0
  .report = ethernet_report,
#endif
};
