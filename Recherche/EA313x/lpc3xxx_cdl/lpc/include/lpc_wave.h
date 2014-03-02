/***********************************************************************
 * $Id:: lpc_wave.h 6114 2011-01-11 04:42:35Z ing02124                 $
 *
 * Project: Wave file definitions
 *
 * Description:
 *     This file contains the structure definitions and manifest
 *     constants for the following component:
 *         Wave file formats
 *
 ***********************************************************************
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * products. This software is supplied "AS IS" without any warranties.
 * NXP Semiconductors assumes no responsibility or liability for the
 * use of the software, conveys no license or title under any patent,
 * copyright, or mask work right to the product. NXP Semiconductors
 * reserves the right to make changes in the software without
 * notification. NXP Semiconductors also make no representation or
 * warranty that such application will be suitable for the specified
 * use without further testing or modification.
 **********************************************************************/

#ifndef LPC_WAVE_H
#define LPC_WAVE_H

#include "lpc_types.h"

#if defined (__cplusplus)
extern "C"
{
#endif

/***********************************************************************
 * Defines
 **********************************************************************/

#define RIFF_CHUNK_ID     "RIFF"   /* ID name for the RIFF chunk */
#define FORMAT_CHUNK_ID   "fmt "   /* ID name for the format chunk */
#define DATA_CHUNK_ID     "data"   /* ID name for a data chunk */
#define CHUNK_ID_SIZE     4        /* Size of a chunk name ID */

#define WAVE_TAG_ID       "WAVE"   /* ID name for 'wave' name check */
#define WAVE_TAG_SIZE     4        /* Size of the 'wave' ID */

#define WTAG_NOCOMPRESS   0x1      /* Uncompressed audio tag for
wformattag */

/***********************************************************************
 * Types
 **********************************************************************/
/* Chunk Type Enum */
typedef enum
{
  FORMAT_CHUNK,
  DATA_CHUNK
}	CHUNK_TYPE;

/* Format Tag Enum */
typedef enum
{
  WAVE_PCM = 0x0001,   // Supports Raw PCM files only
}	FORMAT_TAG;

/* Structure containing the information of a RIFF chunk */
typedef struct
{
  CHAR     id[4];        /* Should be "riff " */
  UNS_32   chunksize;    /* Total size of the file (minus 8) */
  CHAR     waveid[4];    /* Should be "wave " */
} RIFF_CHUNK_T;

/* Structure that contains the format chunk information */
typedef struct
{
  CHAR     id[4];        /* Should be "fmt " */
  UNS_32   chunksize;    /* Size of this chunk (minus 8) */
  INT_16   wformattag;   /* Format tag (0 = uncompressed) */
  UNS_16   wchannels;    /* # of channels (1 = mono/2 = stereo) */
  UNS_32   samplessec;   /* Samples per second (frequency) */
  UNS_32   bytespersec;  /* Bytes per second */
  UNS_16   blockalign;   /* Single sample size in bytes */
  UNS_16   bitssample;   /* Bits per samples */
} FORMAT_CHUNK_T;

/* Structure that contains the data chunk information */
typedef struct
{
  CHAR     id[4];        /* Should be "data" */
  UNS_32   chunksize;    /* Size of this chunk (minus 8) */
} DATA_CHUNK_T;

/* LPC wave file reader functions */
BOOL_32	lpc_check_riff_chunk(RIFF_CHUNK_T *chunk);

/* Check the chunk type - Format or Data chunk */
STATUS	lpc_check_chunk_type(DATA_CHUNK_T *chunk,
                            CHUNK_TYPE *type);

/* Check Format chunk validity */
STATUS lpc_check_format_chunk(FORMAT_CHUNK_T *chunk);

#if defined (__cplusplus)
}
#endif /*__cplusplus */

#endif /* LPC_WAVE_H */
