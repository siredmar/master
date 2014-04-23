/* tftp.c

   written by Marc Singer
   8 Jul 2005

   Copyright (C) 2005 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

   The tftp descriptor looks like this:

     tftp://host/path
   or
     tftp:/path using a default host of $server, perhaps.


  NOTES
  -----

   o The meshing o the tftp protocol and data handling within APEX is
     a bit more complex that would be ideal.  tftp only considers
     streams of bytes to be accessible linearly from the start to the
     end.  There is no seeking.  There is a timeout on each block, so
     APEX cannot hold off indefinitely with continuing the stream
     transfer.  The only flow control that APEX can assert when
     reading files is to throttle the acknowlegements.  So, what it
     shall do is request the first block.  Let the next layer read
     data from the buffered data and wait for the whole block to be
     consumed before acknowledging that the block was received.

   o Performance may be improved by caching  data blocks and allowing
     there to be a sliding window of available data.  This could,
     potentially, allow for a reasonable overlap of requests and
     processing.  Especially if APEX is performing some sort of
     transformation on the data, e.g. checksum or writing to
     flash.  For transfers to memory, there is probably little to be
     gained with a sliding window.  However, this driver doesn't know
     how the data will be used.

     The throughput on the 79520 target as compared to a fast x86
     notbook is noticeable.  It is possible that we'll get something
     from a faster turnaround and overlapped acks.  As this isn't a
     serious proble, considering the way that tftp is used, I'll leave
     it as is.

   o We need to select a random port number.  We need to see if there
     is some place where we can sample semi-random data.  At least so
     that we have a chance to detect out-dated connections.  Or, we
     can let this be a problem for the udp layer.

   o Error packets.  We don't ever send or interpret error packets.
     These are valid in several instances, but the timeouts make them
     an optional.  Note that the protocol doesn't consider them
     optional, but because error packets may be lost, there is no way
     to know if this client is compliant.

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
#include <console.h>

#include <network.h>
#include <ethernet.h>

//#define TALK 2
#include <talk.h>

#define DRIVER_NAME	"tftp"

#define MS_TIMEOUT	(1*1000)
#define RETRIES_MAX	10

#define BLOCK_LENGTH	(512)
#define BLOCKS_CACHED	(4)	/* Number of blocks in the cache */

enum {
  stateIdle = 0,
  stateOpenWaiting,
  stateBlockAvailable,
  stateAck,
  stateWaiting,
  stateBlockFinal,
  stateError,
  stateAbort,
};

struct tftp_info {
  struct descriptor_d d;	/* ethernet device */
  int state;
  char server_ip[4];
  int source_port;
  int destination_port;
  int mode;			/* reading or writing, uses an opcode */
  int block;			/* block number being read */
  int blockRec;			/* block number of last received block */
  size_t cbRec;		/* count of bytes received */
  unsigned char rgb[BLOCK_LENGTH*BLOCKS_CACHED];
  struct ethernet_frame* frame;
  int cRetries;			/* Number of re-acks */
};

struct tftp_info tftp;

static int tftp_receiver (struct descriptor_d* d,
			  struct ethernet_frame* frame,
			  void* context)
{
  struct tftp_info* info = (struct tftp_info*) context;
  u16 opcode;
  size_t cb;
  u16 block;

  DBG (2,"tftp_receiver %d %d\n", info->blockRec, info->cbRec);

	/* Vet the frame */
  if (frame->cb < (sizeof (struct header_ethernet)
		   + sizeof (struct header_ipv4)
		   + sizeof (struct header_udp)
		   + sizeof (struct message_tftp)))
    return 0;			/* runt */
  if (ETH_F (frame)->protocol == HTONS (ETH_PROTO_IP)
      && IPV4_F (frame)->protocol == IP_PROTO_UDP) {
    DBG (2,"  dp %d  sp %d\n",
	 htons (UDP_F (frame)->destination_port),
	 htons (UDP_F (frame)->source_port));
  }

  if (   ETH_F (frame)->protocol != HTONS (ETH_PROTO_IP)
      || IPV4_F (frame)->protocol != IP_PROTO_UDP
      || htons (UDP_F (frame)->destination_port) != info->source_port)
    return 0;

#if defined (CONFIG_UDP_CHECKSUM)
  if (udp_checksum_verify (frame)) {
    printf ("checksum failed\n");
    return 1;			/* Discard */
  }
#endif

  opcode = htons (TFTP_F (frame)->opcode);

  if (opcode == TFTP_DATA && info->blockRec == 0)
    info->destination_port = htons (UDP_F (frame)->source_port);

#if 0
  {
    static int c;
    /* Code to test dropping of packets */
    if ((c++ % 25) == 0) {
    /* Drop every 25th packet */
    //    printf ("drop\n");
      return 0;
    }
  }
#endif

  switch (opcode) {
  case TFTP_DATA:
    block = htons (*(u16*) TFTP_F (frame)->data);
    DBG (1,"tftp data (3) block %d\n", block);

    if (block != info->blockRec + 1) { /* out-of-sync */
      info->state = stateAck;
      break;
    }

    cb = htons (UDP_F (frame)->length) - sizeof (struct header_udp)
      - sizeof (struct message_tftp) - 2;
    memcpy (&info->rgb[info->cbRec % sizeof (info->rgb)],
	    TFTP_F (frame)->data + 2, cb);
    ++info->blockRec;
    info->cbRec += cb;
    info->state = (cb == 512 ? stateBlockAvailable : stateBlockFinal);
    DBG (1,"received %d of %d bytes  block %d (%d)\n",
	 cb, info->cbRec, info->blockRec, info->state);
    break;

  case TFTP_ERROR:
    {
#if !defined (CONFIG_SMALL)
      int cb = frame->cb - (sizeof (struct header_ethernet)
                            + sizeof (struct header_ipv4)
                            + sizeof (struct header_udp)
                            + sizeof (struct message_tftp))
        - sizeof (u16);
      printf ("tftp error: (%d) %-*.*s\n",
              htons (*(u16*) TFTP_F (frame)->data),
              cb, cb, TFTP_F (frame)->data + 2);
#endif
      tftp.state = stateError;
    }
    break;

  default:
    DBG (1,"tftp response opcode %d\n", opcode);
    break;
  }

  return 1;
}


/* tftp_terminate

   is the function used by ethernet_service() to deterine when to
   terminate the loop.  It return zero when the loop can continue, -1
   on timeout, and 1 when the configuration is complete.

*/

static int tftp_terminate (void* pv)
{
  struct ethernet_timeout_context* context
    = (struct ethernet_timeout_context*) pv;

  /* *** FIXME: ouch */
  if (tftp.state != stateOpenWaiting && tftp.state != stateWaiting)
    return 1;

  if (!context->time_start)
    context->time_start = timer_read ();

  if (console->poll (0, 0))
    return -2;			/* Not really a timeout */

  return timer_delta (context->time_start, timer_read ()) < context->ms_timeout
    ? 0 : -1;
}

static ssize_t tftp_read (struct descriptor_d* d, void* pv, size_t cb)
{
  int result;
  size_t cbRead = 0;

  while (cb) {
    int available = tftp.cbRec - (d->start + d->index);

    switch (tftp.state) {
    case stateIdle:		/* Need to read data */

      tftp.source_port = port_allocate ();
      tftp.destination_port = 69;
      tftp.mode = TFTP_RRQ;
      tftp.frame = ethernet_frame_allocate ();

	/* -- Initiate transfer -- */

      TFTP_F (tftp.frame)->opcode = htons (tftp.mode);
      {
	char* pch = (char*) TFTP_F(tftp.frame)->data;
	size_t cb = strlcpy (pch, d->pb[d->iRoot], 400) + 1;
	strcpy (pch + cb, "octet");
	cb += strlen (pch + cb) + 1;
	udp_setup (tftp.frame, tftp.server_ip, tftp.destination_port,
		   tftp.source_port, sizeof (struct message_tftp) + cb);
      }

      tftp.d.driver->write (&tftp.d, tftp.frame->rgb, tftp.frame->cb);
      tftp.state = stateOpenWaiting;
      break;

    case stateOpenWaiting:
    case stateWaiting:
      {
	struct ethernet_timeout_context timeout;

	memset (&timeout, 0, sizeof (timeout));
	timeout.time_start = 0;
	timeout.ms_timeout = MS_TIMEOUT;
	result = ethernet_service (&tftp.d, tftp_terminate, &timeout);

	/* *** need to check that we received a block, otherwise, the
	   connection cannot be initiated */

	if (result == -2) {
	  tftp.state = stateAbort;
	  cbRead = ERROR_BREAK;
	  DBG (1, "%s: abort\n", __FUNCTION__);
	  goto quit;
	}

	if (result < 0) {	/* Timeout */

//	  printf ("tftp timeout\n");
	  if (++tftp.cRetries >= RETRIES_MAX) {
	    tftp.state = stateError;
	    DBG (1, "%s: timeout error\n", __FUNCTION__);
	    goto quit;
	  }
	  if (tftp.state == stateOpenWaiting)
	    tftp.state = stateIdle;
	  else
	    tftp.state = stateAck;
	  break;
	}

	if (   tftp.state != stateBlockAvailable
	    && tftp.state != stateBlockFinal) {
	  DBG (1, "%s: probably no file\n", __FUNCTION__);
	  goto quit;		/* Terminate, probably no such file */
	}

      }
      break;

    case stateBlockFinal:
      if (available == 0) {
	DBG (1, "%s: stateBlockFinal\n", __FUNCTION__);
	goto quit;
      }
      /* fall through */

    case stateBlockAvailable:
      if (available == 0) {
	tftp.state = stateAck;
	break;
      }

      if (available > cb)
	available = cb;
      memcpy (pv, tftp.rgb + (d->start + d->index)%sizeof (tftp.rgb),
	      available);
      d->index += available;
      cb -= available;
      cbRead += available;
      pv += available;
      break;

    case stateAck:
      DBG (1, "acking %d\n", tftp.blockRec);
      TFTP_F (tftp.frame)->opcode = htons (TFTP_ACK);
      *(u16*) TFTP_F (tftp.frame)->data = htons (tftp.blockRec);
      udp_setup (tftp.frame, tftp.server_ip, tftp.destination_port,
		 tftp.source_port, sizeof (struct message_tftp) + 2);
      usleep (1000);
      tftp.d.driver->write (&tftp.d, tftp.frame->rgb, tftp.frame->cb);
      tftp.state = stateWaiting;
      break;

    case stateAbort:
      cbRead = ERROR_BREAK;
      DBG (1, "%s: stateAbort\n", __FUNCTION__);
      goto quit;
    }
  }

 quit:
  return cbRead;
}


/* tftp_open

   the only thing this can do is verify whether or not it is
   reasonable to open the file.  We could do some vetting of the
   addresses, perhaps check the host IP address.  Maybe we could ARP
   for the server to make sure it is accessible.

*/

static int tftp_open (struct descriptor_d* d)
{
  int result;
  extern const char szNetDriver[];

  DBG (2,"%s: d->c %d d->iRoot %d '%s' '%s'\n",
	  __FUNCTION__, d->c, d->iRoot, d->pb[0], d->pb[1]);

  if (UNCONFIGURED_IP)
    ERROR_RETURN (ERROR_FAILURE, "IP address not configured");

  if (d->c != 2)
    ERROR_RETURN (ERROR_FILENOTFOUND, "invalid path");
  if (d->iRoot != 1)
    ERROR_RETURN (ERROR_FILENOTFOUND, "server IP required");

  memset (&tftp, 0, sizeof (tftp)); /* clobber transfer state */

  if ((result = parse_descriptor (szNetDriver, &tftp.d)))
    return result;

  result = getaddr (d->pb[0], tftp.server_ip);
  if (result)
    return result;

  if (!arp_resolve (&tftp.d, tftp.server_ip, 0))
    ERROR_RETURN (ERROR_PARAM, "no route to host");

  if ((result = open_descriptor (&tftp.d)))
    return result;

  d->length = DRIVER_LENGTH_MAX; /* We don't know the length, so make it big */

  register_ethernet_receiver (100, tftp_receiver, &tftp);

  return 0;
}


static void tftp_close (struct descriptor_d* d)
{
  if (tftp.frame) {
    ethernet_frame_release (tftp.frame);
    tftp.frame = NULL;
  }

  unregister_ethernet_receiver (tftp_receiver, &tftp);
  close_descriptor (&tftp.d);

  close_helper (d);
}


/* tftp_seek

   very limited seek function.

*/

static driver_off_t tftp_seek (struct descriptor_d* d,
                               driver_off_t cb, int whence)
{
  if (cb < 0 || whence != SEEK_CUR)
    return 0;			/* *** FIXME */

  while (cb) {
    int available = tftp.cbRec - (d->start + d->index);
    if (available > cb)
      available = cb;

    if (!available) {
      char ch;
      available = d->driver->read (d, &ch, 1);
      if (!available)
	return 0;		/* *** FIXME: error condition? */
    }

    cb -= available;
    d->index += available;
  }

  return 0;
}


#if 0
static ssize_t tftp_write (struct descriptor_d* d, void*, size_t cb)
{
  return 0;
}
#endif

static __driver_6 struct driver_d tftp_driver = {
  .name = DRIVER_NAME,
  .description = "trivial FTP driver",
  .flags = DRIVER_DESCRIP_FS | DRIVER_DESCRIP_SIMPLEPATH
  | DRIVER_WRITEPROGRESS(6) | DRIVER_READPROGRESS(6),
  .open = tftp_open,
  .close = tftp_close,
  .read = tftp_read,
//  .write = tftp_write,
  .seek = tftp_seek,
#if defined CONFIG_CMD_INFO
//  .info = tftp_info,
#endif
};
