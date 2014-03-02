
#ifndef STD_TYPES_H
#define STD_TYPES_H

#include "platform_types.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>



typedef uint8 Std_ReturnType;

#define E_OK		  (0x00U)     /**<\brief Standard error code for no error */
#define E_NOT_OK	  (0x01U)     /**<\brief Standard error code for error */

#define STD_HIGH    (0x01U)     /**<\brief Physical state 5V or 3.3V */
#define STD_LOW     (0x00U)     /**<\brief Physical state 0V */

#define STD_ACTIVE  (0x01U)     /**<\brief Logical state active */
#define STD_IDLE    (0x00U)     /**<\brief Logical state idle */

#define STD_ON      (0x01U)     /**<\brief Standard ON type */
#define STD_OFF     (0x00U)     /**<\brief Standard OFF type */

#define STD_ENABLE  (0x01U)     /**<\brief Standard ENABLE type */
#define STD_DISABLE (0x00U)     /**<\brief Standard DISABLE type */


#endif /* STD_TYPES_H */


