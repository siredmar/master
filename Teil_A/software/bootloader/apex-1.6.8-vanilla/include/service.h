/* service.h

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

#if !defined (__SERVICE_H___)
#    define   __SERVICE_H___

/* ----- Includes */

#include <attributes.h>

/* ----- Types */

struct service_d {
  void (*init) (void);
  void (*report) (void);	/* Informative output from service */
  void (*release) (void);
//  void (*service) (void);	/* Background service */
};

#define __service_0 __used __section(.service.0) /* target */
#define __service_1 __used __section(.service.1) /* exception & mmu*/
#define __service_2 __used __section(.service.2) /* timer */

#define __service_3 __used __section(.service.3) /* serial */
#define __service_4 __used __section(.service.4) /* drv-mem */
#define __service_5 __used __section(.service.5)
#define __service_6 __used __section(.service.6) /* drv-*, ethernet */
#define __service_7 __used __section(.service.7) /* env */
#define __service_8 __used __section(.service.8)
#define __service_9 __used __section(.service.9) /* variation */

/* ----- Globals */

/* ----- Prototypes */

extern void init_services (void);
extern void release_services (void);

#endif  /* __SERVICE_H___ */
