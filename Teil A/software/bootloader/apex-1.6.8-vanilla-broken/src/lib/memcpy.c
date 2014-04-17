/* memcpy.c

   refer to string.c for the copyright and notes.

 */

#define IN_STRING_C 1

#include <linux/types.h>
#include <linux/string.h>
#include <linux/ctype.h>
#include <linux/module.h>

#define BUG_ON(v)

/**
 * memcpy - Copy one area of memory to another
 * @dest: Where to copy to
 * @src: Where to copy from
 * @count: The size of the area.
 *
 * You should not use this function to access IO space, use memcpy_toio()
 * or memcpy_fromio() instead.
 */
void * memcpy(void * dest,const void *src,size_t count)
{
	char *tmp = (char *) dest, *s = (char *) src;

	while (count--)
		*tmp++ = *s++;

	return dest;
}
EXPORT_SYMBOL(memcpy);
