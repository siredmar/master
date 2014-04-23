/* strnlen.c

   refer to string.c for the copyright and notes.

 */


#define IN_STRING_C 1

#include <linux/types.h>
#include <linux/string.h>
#include <linux/ctype.h>
#include <linux/module.h>

#define BUG_ON(v)

/**
 * strnlen - Find the length of a length-limited string
 * @s: The string to be sized
 * @count: The maximum number of bytes to search
 */
size_t strnlen(const char * s, size_t count)
{
	const char *sc;

	for (sc = s; count-- && *sc != '\0'; ++sc)
		/* nothing */;
	return sc - s;
}
EXPORT_SYMBOL(strnlen);
