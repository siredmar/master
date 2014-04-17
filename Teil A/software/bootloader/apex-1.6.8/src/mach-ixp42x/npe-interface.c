/* npe-interface.c

   written by Marc Singer
   8 Mar 2005

   Copyright (C) 2005 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

   Interface file that links compiles with the access library headers.
   This code is separated because of the extensive conflicts between
   the access library and the bootloader.

*/

#include <IxNpeDl.h>
#include <IxQMgr.h>
#include <IxNpeMh.h>
#include <IxEthAcc.h>
#include <ix_ossl.h>

//#include <_intel/include/npe_info.h>
#include <IxNpeMicrocode.c>

#undef printf
extern int __attribute__((format (printf, 1, 2)))
     printf (const char * fmt, ...);

#define NPE_VERSIONS_COUNT 12

extern const unsigned IX_NPEDL_MicrocodeImageLibrary[];

static int npeDownload (int npeId, int functionalityId)
{
  UINT32 i;
  UINT32 c;
  IxNpeDlVersionId list[NPE_VERSIONS_COUNT];
  IxNpeDlVersionId dlVersion;
  int major, minor;

  if (ixNpeDlAvailableVersionsCountGet (&c) != IX_SUCCESS)
    return 1;

  if (c > NPE_VERSIONS_COUNT)
    return 1;

  if (ixNpeDlAvailableVersionsListGet (list, &c) != IX_SUCCESS)
    return 1;

  major = minor = 0;
  for (i = 0; i < c; i++) {
    if (list[i].npeId == npeId) {
      if (list[i].functionalityId == functionalityId) {
	if (list[i].major > major) {
	  major = list[i].major;
	  minor = list[i].minor;
	} else if (list[i].major == major && list[i].minor > minor)
	  minor = list[i].minor;
      }
    }
  }

  printf ("NPE%c major[%d] minor[%d] build[%d]\n",
	  npeId  + 'A', major, minor, buildId);


    if (ixNpeDlNpeStopAndReset(npeId) != IX_SUCCESS) {
      printf("npeDownload: Failed to stop/reset NPE%c\n", npeId  + 'A');
      return 1;
    }
    dlVersion.npeId = npeId;
    dlVersion.buildId = buildId;
    dlVersion.major = major;
    dlVersion.minor = minor;

    if (ixNpeDlVersionDownload(&dlVersion, 1) != IX_SUCCESS) {
      printf("Failed to download to NPE%c\n", npeId  + 'A');
      return 1;
    }


#if 0				/* Verify */
    // verify download
    if (ixNpeDlLoadedVersionGet(npeId, &dlVersion) != IX_SUCCESS) {
      printf ("Failed to upload version from NPE%c\n", npeId  + 'A');
      return 1;
    }

    if (   dlVersion.buildId != buildId
	|| dlVersion.major != major
	|| dlVersion.major != major) {
      printf ("Failed to verify download NPE%c\n", npeId  + 'A');
      return 1;
    }
#endif

    return 0;

}

int _npe_init (void)
{
//  const unsigned* pl = IX_NPEDL_MicrocodeImageLibrary;
//  volatile unsigned l = *pl;

#if 0
  npe_alloc_end = npe_alloc_pool + sizeof(npe_alloc_pool);
  npe_alloc_free = (cyg_uint8 *)(((unsigned)npe_alloc_pool
				  + HAL_DCACHE_LINE_SIZE - 1)
				 & ~(HAL_DCACHE_LINE_SIZE - 1));
#endif

  if (ixQMgrInit() != IX_SUCCESS) {
    printf ("Error initialising queue manager!\n");
    return 1;
  }

  if (!npeDownload(0, 0))
    return 1;

  if (!npeDownload(1, 0))
    return 1;

  return 0;
}
