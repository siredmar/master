/* region-copy.c

   written by Marc Singer
   22 Aug 2008

   Copyright (C) 2008 Marc Singer

   -----------
   DESCRIPTION
   -----------

   Region copy routine for use by other commands.  This code was taken
   from the original cmd-copy.c and augmented with a flags parameter.

*/

#include <config.h>
#include <apex.h>
#include <driver.h>
#include <error.h>
#include <spinner.h>
#include "region-copy.h"
#include <asm/byteorder.h>


#if !defined (CONFIG_SMALL)
//# define USE_COPY_VERIFY	/* Define to include verify feature */
#endif


/** region_copy copied from region din to dout.  The regions must
    already be open.  The flags parameter comes from the enumeration
    in region-copy.h.  Verify requires that the USE_COPY_VERIFY macro
    be set. */

int region_copy (struct descriptor_d* dout, struct descriptor_d* din,
                 unsigned flags)
{
#if defined (USE_COPY_VERIFY)
  struct descriptor_d din_v;
  struct descriptor_d dout_v;
#endif
  ssize_t cbCopied = 0;

  /* Make sure we try to copy the no more than either descriptor can
     handle. */
  ssize_t cbCopy = din->length - din->index;
  if (cbCopy > dout->length - dout->index)
    cbCopy = dout->length - dout->index;

#if defined (USE_COPY_VERIFY)
  /* Create descriptors for rereading and verification */
  /* *** FIXME: we ought to perform a dup () */
  memcpy (&din_v, din, sizeof (*din));
  memcpy (&dout_v, dout, sizeof (*dout));
  dout_v.length = din_v.length;
#endif

  {
    char __aligned rgb[512];
    ssize_t cb;
    size_t available;
    int report_last = -1;
    int step = DRIVER_PROGRESS (din, dout);
    if (step)
      step += 10;

#define AVAILABLE(c,s) (((c) < (s)) ? c : s)

    for (available = AVAILABLE (cbCopy, sizeof (rgb)) ;
         (cb = din->driver->read (din, rgb, available)) > 0;
	 cbCopy -= cb, cbCopied += cb,
           available = AVAILABLE (cbCopy, sizeof (rgb))) {
      int report;
      size_t cbWrote;
      if (cb == 0)
	ERROR_RETURN (ERROR_FAILURE, "premature end of input");

#if defined (USE_COPY_VERIFY)
      if (flags & regionCopyVerify) {
	char __aligned rgbVerify[512];
	ssize_t cbVerify = din_v.driver->read (&din_v, rgbVerify,
					       sizeof (rgbVerify));
	if (cbVerify != cb) {
          if (!(flags & regionCopyQuiet))
            printf ("\rVerify failed: reread of input %d, expected %d, at"
                    " 0x%x+0x%x\n", cbVerify, cb, cbCopied, 512);
	  return ERROR_FAILURE;
	}
	if (memcmp (rgb, rgbVerify, cb)) {
          if (!(flags & regionCopyQuiet))
            printf ("\rVerify failed: reread input compare at 0x%x+0x%x\n",
                    cbCopied, 512);
	  return ERROR_FAILURE;
	}
      }
#endif

      if (flags & regionCopySwap) {
	int i;
	unsigned long* p = (unsigned long*) rgb;
	for (i = cb/4; i-- > 0; ++p)
	  *p = swab32 (*p);
      }

      if (flags & regionCopySpinner)
        SPINNER_STEP;
      cbWrote = dout->driver->write (dout, rgb, cb);
      if (cbWrote != cb)
	ERROR_RETURN (ERROR_FAILURE, "truncated write");

#if defined (USE_COPY_VERIFY)
      if (verify) {
	char rgbVerify[512];
	ssize_t cbVerify = dout_v.driver->read (&dout_v, rgbVerify,
						sizeof (rgbVerify));
	if (cbVerify != cb) {
          if (flags & regionCopyReportVerifyErrors)
            printf ("\rVerify failed: reread of output %d, expected %d, at"
                    " 0x%x+0x%x\n", cbVerify, cb, cbCopied, 512);
	  return ERROR_FAILURE;
	}
	if (swap) {
	  int i;
	  unsigned long* p = (unsigned long*) rgbVerify;
	  for (i = cb/4; i-- > 0; ++p)
	    *p = swab32 (*p);
	}
	if (memcmp (rgb, rgbVerify, cb)) {
          if (flags & regionCopyReportVerifyErrors)
            printf ("\rVerify failed: reread output compare at 0x%x+0x%x\n",
                    cbCopied, 512);
	  return ERROR_FAILURE;
	}
      }
#endif

      report = cbCopied>>step;
      if ((flags & regionCopySpinner) && step && report != report_last) {
	printf ("\r   %d KiB\r", cbCopied/1024);
	report_last = report;
      }
    }

    if (cb < 0)
      ERROR_RETURN (ERROR_FAILURE, "copy overrun");
  }

  return cbCopied;
}
