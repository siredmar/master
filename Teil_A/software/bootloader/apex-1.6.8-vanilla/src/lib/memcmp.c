/* memcmp.c

   refer to string.c for the copyright and notes.

 */

#define IN_STRING_C 1

#include <linux/types.h>
#include <linux/string.h>
#include <linux/ctype.h>
#include <linux/module.h>

#define BUG_ON(v)

/**
 * memcmp - Compare two areas of memory
 * @cs: One area of memory
 * @ct: Another area of memory
 * @count: The size of the area.
 */
int memcmp(const void * cs,const void * ct,size_t count)
{
	const unsigned char *su1, *su2;
	int res = 0;

	for( su1 = cs, su2 = ct; 0 < count; ++su1, ++su2, count--)
		if ((res = *su1 - *su2) != 0)
			break;
	return res;
}
EXPORT_SYMBOL(memcmp);
