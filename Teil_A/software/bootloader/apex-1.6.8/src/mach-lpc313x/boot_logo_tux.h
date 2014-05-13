/* Image in RGB565 format, width: 300, height: 150 */

#ifndef BOOT_LOGO_H_
#define BOOT_LOGO_H_

#include "hardware.h"

#if defined(CONFIG_LOGO_TUX)
u16 boot_logo_tux[];
#endif
#endif /* BOOT_LOGO_H_ */
