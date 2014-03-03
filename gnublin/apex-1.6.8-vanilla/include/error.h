/* error.h

   written by Marc Singer
   3 Nov 2004

   Copyright (C) 2004 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

*/

#if !defined (__ERROR_H__)
#    define   __ERROR_H__

extern const char* error_description;

#define ERROR_NONE		(0)	/* It isn't necessary to use this */
#define ERROR_FALSE		(-1)
#define ERROR_IMPORTANT		(-1)	/* Errors less than will be reported */
#define ERROR_BREAK		(-2)	/* User requested break */
#define ERROR_FAILURE		(-7)
#define ERROR_NOCOMMAND		(-8)
#define ERROR_PARAM		(-9)
#define ERROR_OPEN		(-10)
#define ERROR_AMBIGUOUS		(-11)
#define ERROR_NODRIVER		(-12)
#define ERROR_UNSUPPORTED	(-13)
#define ERROR_BADPARTITION	(-14)
#define ERROR_FILENOTFOUND	(-15)
#define ERROR_IOFAILURE		(-16)
#define ERROR_CRCFAILURE	(-17)
#define ERROR_OUTOFMEMORY	(-18)
#define ERROR_TIMEOUT		(-19)
#define ERROR_UNRECOGNIZED	(-20)

#if defined (CONFIG_SMALL)
# define ERROR_RETURN(v,m) return (v)
# define ERROR_RESULT(v,m) (v)
#else
# define ERROR_RETURN(v,m) ({ error_description = (m); return (v); })
# define ERROR_RESULT(v,m) ({ error_description = (m); (v); })
#endif

#endif  /* __ERROR_H__ */
