/***********************************************************************
 * $Id:: lpc_wave.c 6114 2011-01-11 04:42:35Z ing02124               $
 *
 * Project: WAVE file reader utility
 *
 * Description:
 *     WAVE file reader functions
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

#include "lpc_wave.h"
#include "lpc_string.h"

/* Supported Bit Rates for CODEC */
static const UNS_32 samp_rates[] = { 8000, 11025, 16000,
                                     22050, 44100, 48000
                                   };

/***********************************************************************
 *
 * Function: lpc_check_riff_chunk
 *
 * Purpose: Read RIFF chunk data
 *
 * Processing:
 *     Read RIFF chunk data from buffer & check if it is valid WAVE file.
 *
 * Parameters:
 *     chunk : Pointer to RIFF chunk structure
 *
 * Outputs: None
 *
 * Returns: TRUE - WAVE file found
 * 	 				FALSE - Wave file not found
 *
 * Notes: None
 *
 **********************************************************************/
BOOL_32 lpc_check_riff_chunk(RIFF_CHUNK_T *chunk)
{
  BOOL_32 found = TRUE;

  /* Check if valid WAVE File */
  if ((str_ncmp((void *)chunk->id, "RIFF", 4))  ||
      (str_ncmp((void *)chunk->waveid, "WAVE", 4)))
  {
    found = FALSE;
  }

  return found;
}

/***********************************************************************
 *
 * Function: lpc_check_chunk_type
 *
 * Purpose: Read chunk info & validate
 *
 * Processing:
 *     Read chunk info from buffer & check if it is valid chunk type.
 *
 * Parameters:
 *     chunk : Pointer to chunk structure
 *		 type: Pointer to chunk type
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
STATUS lpc_check_chunk_type(DATA_CHUNK_T *chunk,
                            CHUNK_TYPE *type)
{
  BOOL_32 status = _NO_ERROR;

  /* Check if valid format or data chunk
   Raw PCM file will have Format & Data chunks */
  if (!(str_ncmp((void *)chunk->id, "fmt ", 4)))
  {
    *type = FORMAT_CHUNK;
  }
  else if (!(str_ncmp((void *)chunk->id, "data", 4)))
  {
    *type = DATA_CHUNK;
  }
  else
  {
    status = _ERROR;
  }

  return status;
}

/***********************************************************************
 *
 * Function: lpc_check_format_chunk
 *
 * Purpose: Check Format chunk data
 *
 * Processing:
 *     Read Format chunk data from buffer & check if valid format data.
 *
 * Parameters:
 *     chunk : Pointer to Format chunk structure
 *
 * Outputs: None
 *
 * Returns: TRUE - WAVE file found
 * 	 				FALSE - Wave file not found
 *
 * Notes: None
 *
 **********************************************************************/
STATUS lpc_check_format_chunk(FORMAT_CHUNK_T *chunk)
{
  STATUS status = _NO_ERROR;
  BOOL_32 valid = FALSE;
  UNS_32 i, samp_size;

  /* Check validity of Audio Format
   Only Raw PCM format files are supported */
  switch (chunk->wformattag)
  {
    case WAVE_PCM:
      break;
    default:
      status = _ERROR;
      break;
  }

  if (status == _NO_ERROR)
  {
    /* Check Mono or Stereo */
    if ((chunk->wchannels != 1) &&
        (chunk->wchannels != 2))
    {
      status = _ERROR;
      goto error;
    }

    /* Check bits per sample */
    if ((chunk->bitssample != 8) &&
        (chunk->bitssample != 16))
    {
      status = _ERROR;
      goto error;
    }

    /* Check if Sample rate is supported */
    samp_size = sizeof(samp_rates) / sizeof(UNS_32);
    for (i = 0; i < samp_size; i++)
    {
      if (chunk->samplessec == samp_rates[i])
      {
        valid = TRUE;
        break;
      }
    }

    if (valid == FALSE)
    {
      status = _ERROR;
    }
  }

error:
  return status;
}
