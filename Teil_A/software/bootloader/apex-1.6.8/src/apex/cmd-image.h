/* cmd-image.h

   written by Marc Singer
   12 Sep 2008

   Copyright (C) 2008 Marc Singer

   -----------
   DESCRIPTION
   -----------

*/

#if !defined (__CMD_IMAGE_H__)
#    define   __CMD_IMAGE_H__

/* ----- Includes */

#include <driver.h>

/* ----- Types */

struct image_info {
  bool fRegionCanExpand;
  uint32_t initrd_relocation;
};

typedef int (*pfn_handle_image) (int, struct descriptor_d*, struct image_info*);

/* ----- Globals */

/* ----- Prototypes */

pfn_handle_image is_apex_image (const char*, size_t);
pfn_handle_image is_uboot_image (const char*, size_t);


#endif  /* __CMD_IMAGE_H__ */
