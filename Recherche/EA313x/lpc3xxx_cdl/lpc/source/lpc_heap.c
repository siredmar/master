/***********************************************************************
 * $Id:: lpc_heap.c 745 2008-05-13 19:59:29Z pdurgesh                  $
 *
 * Project: Simple heap manager
 *
 * Description:
 *     See the header file for a description of this package.
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

#include "lpc_heap.h"

/***********************************************************************
 * Package types
 **********************************************************************/

/* Heap descriptor */
typedef struct
{
  UNS_32 entry_size;       /* Size of this heap entry including
                                the descriptor (0 = used) */
  void *next_descriptor;   /* Pointer to next descriptor (0 = last) */
  void *prev_descriptor;   /* Pointer to previous descriptor
                                (heap_base = no previous entry) */
} HEAP_DESCRIPTOR_T;

/***********************************************************************
 * Package data
 **********************************************************************/

/* Heap base address */
HEAP_DESCRIPTOR_T *heap_base;
/* Heap size */
UNS_32 heap_size_saved;

/***********************************************************************
 * Package defines
 **********************************************************************/

/* Pointer to NULL heap descriptor */
#define HEAP_POINTER_NULL   ((HEAP_DESCRIPTOR_T *) 0)
/* Heap descriptor size */
#define HEAP_HEAD_SIZE      (sizeof (HEAP_DESCRIPTOR_T))
/* Smallest heap descriptor entry */
#define SMALLEST_ENTRY_SIZE (HEAP_HEAD_SIZE + sizeof (UNS_32))

/***********************************************************************
 * Private functions
 **********************************************************************/

/***********************************************************************
 *
 * Function: lpc_find_free_entry
 *
 * Purpose: Finds a chunk of free heap memory with the required size
 *
 * Processing:
 *     The heap list is traversed until an entry with the minimum
 *     required size available is found.
 *
 * Parameters:
 *     size_in_bytes : Byte size of the requested allocation area
 *
 * Outputs: None
 *
 * Returns: A pointer to the chunk with the minimum required size, or
 *          '0' if a chunk does not exist.
 *
 * Notes: None
 *
 **********************************************************************/
HEAP_DESCRIPTOR_T *lpc_find_free_entry(UNS_32 size_in_bytes)
{
  HEAP_DESCRIPTOR_T *found_ptr;
  INT_32 found = 0;

  /* Start at top of list */
  found_ptr = heap_base;

  /* Loop through entries */
  while (found == 0)
  {
    if (found_ptr->entry_size >= (size_in_bytes + HEAP_HEAD_SIZE))
    {
      /* There is enough room in this heap chunk for the required
         data allocation and a new heap descriptor */
      found = 1;
    }
    else
    {
      /* Go to next heap descriptor in the list */
      found_ptr = found_ptr->next_descriptor;

      /* Exit on last descriptor */
      found = (found_ptr == HEAP_POINTER_NULL);
    }
  }

  return found_ptr;
}

/***********************************************************************
 *
 * Function: lpc_heap_insert_entry
 *
 * Purpose: Get an allocated area from the heap
 *
 * Processing:
 *     A data entry in the heap must always have a data size evenly
 *     divided by 4. The size may be adjusted up the meet this size
 *     requirement. A call to 'lpc_find_free_entry' is performed to
 *     determine if and where the next available chunk in the heap
 *     area exists. If a NULL pointer is returned, a valid size chunk
 *     does not exist, and the function will exit with the NULL return
 *     status. If a valid pointer is return, then the pointer indicates
 *     the start of a new available heap chunk entry.
 *
 * Parameters:
 *     size_in_bytes : Where to insert in the linked list
 *
 * Outputs: None
 *
 * Returns: A pointer to the allocated data area (not the heap
 *          descriptor pointer) or NULL if there is no more room for
 *          the data entry.
 *
 * Notes: None
 *
 **********************************************************************/
void *lpc_heap_insert_entry(UNS_32 size_in_bytes)
{
  HEAP_DESCRIPTOR_T *insert_ptr, *save_entry, *new_entry;
  UNS_32 heap_chunk_size, return_address = 0;

  /* If the size is not mod 4, then increase it until it is */
  if ((size_in_bytes & 0x00000003) != 0)
  {
    size_in_bytes = size_in_bytes + 4;
    size_in_bytes = size_in_bytes & 0xFFFFFFFC;
  }

  /* See if a chunk of heap memory with the required size exists */
  insert_ptr = lpc_find_free_entry(size_in_bytes);

  /* Only insert the heap entry if space is available */
  if (insert_ptr != HEAP_POINTER_NULL)
  {
    /* This is the real required chunk size and includes the heap
       descriptor */
    heap_chunk_size = size_in_bytes + sizeof(HEAP_DESCRIPTOR_T);

    /* Determine if the size of this entry plus the size of the
       heap descriptor will leave enough room in this chunk for
       another entry with the minimum heap entry size */
    if ((heap_chunk_size + SMALLEST_ENTRY_SIZE) >
        insert_ptr->entry_size)
    {
      /* Next entry will not have enough room for the heap
         descriptor and a minimum data entry, so use the entire
         heap chunk for this heap entry */
      heap_chunk_size = insert_ptr->entry_size;
    }
    else
    {
      /* This area of heap memory is large enough for another
         chunk after this one is allocated, so insert this chunk
         and set the heap descriptor link to point to the chunk
         after this chunk */

      /* Save link to next entry */
      save_entry = insert_ptr->next_descriptor;

      /* Create the new entry to insert into the chunk (the chunk
         will be broken into a smaller chunk and an allocated
         data entry */
      new_entry = (HEAP_DESCRIPTOR_T *)((UNS_32) insert_ptr +
                                        heap_chunk_size);

      /* Point the new entry to the link where the insert entry
         was originally pointing to */
      new_entry->next_descriptor = save_entry;

      /* Update the size of the new entry to be the size of the
         original entry minus the required and heap descriptor
         sizes */
      new_entry->entry_size = insert_ptr->entry_size -
                              heap_chunk_size;

      /* Link this data entry to new chunk entry */
      insert_ptr->next_descriptor = new_entry;

      /* Link the following entry back to this entry */
      new_entry->prev_descriptor = insert_ptr;

      /* If the next entry after the new entry is not the last
         entry, then updates it previous entry link to point
        to the new entry */
      if (new_entry->next_descriptor != HEAP_POINTER_NULL)
      {
        ((HEAP_DESCRIPTOR_T *)
         (new_entry->next_descriptor))->prev_descriptor =
           new_entry;
      }

      /* Set return address past the heap descriptor */
      return_address = (UNS_32) insert_ptr + HEAP_HEAD_SIZE;
    }

    /* Set this entire chunk as allocated */
    insert_ptr->entry_size = 0;
  }

  return (UNS_32 *) return_address;
}

/***********************************************************************
 *
 * Function: lpc_find_entry
 *
 * Purpose: Finds a chunk of heap memory which matches the passed data
 *          address.
 *
 * Processing:
 *     The data address passed to this search function must be adjusted
 *     by the heap descriptor size to get the address of the heap
 *     descriptor entry. To provide high realiability, the heap list is
 *     traversed instead of directly accessed to find the entry. If the
 *     entry exists, the address of the heap descriptor is returned.
 *
 * Parameters:
 *     data_entry_addr : Address of an allocated data entry in the heap
 *
 * Outputs: None
 *
 * Returns: A pointer to heap entry, or NULL if it does not exist.
 *
 * Notes: None
 *
 **********************************************************************/
HEAP_DESCRIPTOR_T *lpc_find_entry(UNS_32 *data_entry_addr)
{
  HEAP_DESCRIPTOR_T *search_heap_ptr = heap_base;
  HEAP_DESCRIPTOR_T *found_heap_ptr = HEAP_POINTER_NULL;

  /* Loop until match is found or all entries exhausted */
  while ((search_heap_ptr != HEAP_POINTER_NULL) &&
         (found_heap_ptr == HEAP_POINTER_NULL))
  {
    if (search_heap_ptr->entry_size == 0)
    {
      /* This entry is used, does the data address match entry? */
      if ((UNS_32) data_entry_addr ==
          ((UNS_32) search_heap_ptr + HEAP_HEAD_SIZE))
      {
        /* It matches, this entry must be deleted */
        found_heap_ptr = search_heap_ptr;
      }
    }

    search_heap_ptr = search_heap_ptr->next_descriptor;
  }

  return found_heap_ptr;
}

/***********************************************************************
 *
 * Function: lpc_remove_entry
 *
 * Purpose:
 *     Removes the heap data entry with the passed address if it exists
 *
 * Processing:
 *     This function performs restoration of a heap entry into the heap
 *     chunk list. (A chunk is a free area of the heap). Depending on
 *     the data location in the heap list, a different function may be
 *      performed for each case. See the function for more details.
 *
 * Parameters:
 *     heap_entry_addr : Address of a data entry to return to the heap
 *                       chunk list
 *
 * Outputs: None
 *
 * Returns: '1' if the entry was removed, otherwise '0'.
 *
 * Notes: None
 *
 **********************************************************************/
INT_32 lpc_remove_entry(UNS_32 *heap_entry_addr)
{
  HEAP_DESCRIPTOR_T *found_ptr, *next_ptr, *prev_ptr, *saved_ptr;
  INT_32 status = 0;

  /* Determine if the chunk to deallocate is valid */
  found_ptr = lpc_find_entry(heap_entry_addr);

  /* If the chunk is valid, then deallocate it */
  if (found_ptr != HEAP_POINTER_NULL)
  {
    next_ptr = found_ptr->next_descriptor;

    /* There are a number of different permutations of the heap
       entry return function. The following list is are the
       possible choices for the entry:
        * This is the only heap entry (first entry only)
        * This is the first entry, and the next entry is used
        * This is the first entry, and the next entry is empty
        * This is the last entry, and the previous entry is used
        * This is the last entry, and the previous entry is empty
        * The previous entry and next entry are both used
        * The previous entry is not used, and the next entry is used
        * The previous entry is used, and the next entry is not used
        * The previous entry and next entry are both not used */

    /* First or only entry cases */
    if (found_ptr == heap_base)
    {
      if (next_ptr == HEAP_POINTER_NULL)
      {
        /* This is the only heap entry (first entry only)
           Simply restore heap size to full size */
        found_ptr->entry_size = heap_size_saved;
      }
      else if (next_ptr->entry_size == 0)
      {
        /* Next entry is used, simply clear this entry by
           restoring it's size */
        found_ptr->entry_size = (UNS_32) next_ptr -
                                (UNS_32) found_ptr;
      }
      else
      {
        /* Next entry is also empty, so the first entry
           and the next entry need to be merged */

        /* Save pointer to entry after next entry */
        prev_ptr = next_ptr->next_descriptor;

        /* Update the current next entry point to the entry
           after the next entry */
        found_ptr->next_descriptor = prev_ptr;

        /* If the 'new' next descriptor exists, update its
           previous descriptor pointer to point to the entry */
        if (prev_ptr != HEAP_POINTER_NULL)
        {
          prev_ptr->prev_descriptor = found_ptr;

          /* Update size of this entry */
          found_ptr->entry_size = (UNS_32) prev_ptr -
                                  (UNS_32) found_ptr;
        }
        else
        {
          /* This entry is the only entry in the list */
          found_ptr->entry_size = heap_size_saved;
        }
      }
    }

    /* Last entry cases */
    else if (next_ptr == HEAP_POINTER_NULL)
    {
      prev_ptr = found_ptr->prev_descriptor;
      if (prev_ptr->entry_size == 0)
      {
        /* Previous descriptor is used, so just clear the
           present entry */
        found_ptr->entry_size = 0;
      }
      else
      {
        /* Previous entry is not used, so merge this entry
           and the previous entry */
        prev_ptr->entry_size = heap_size_saved -
                               ((UNS_32) prev_ptr - (UNS_32) heap_base);

        /* Previous entry is now the last entry */
        prev_ptr->next_descriptor = HEAP_POINTER_NULL;
      }
    }

    /* Other entry cases (not first or last entry) */
    else
    {
      prev_ptr = found_ptr->prev_descriptor;
      if ((prev_ptr->entry_size == 0) &&
          (next_ptr->entry_size == 0))
      {
        /* Both previous and next entries are allocated, so
           just clear this entry */
        found_ptr->entry_size = (UNS_32) next_ptr -
                                (UNS_32) found_ptr;
      }
      else if ((prev_ptr->entry_size != 0) &&
               (next_ptr->entry_size == 0))
      {
        /* Previous entry not used, next entry is used, so
           merge this entry and the previous entry */

        /* Point previous entry to next entry */
        prev_ptr->next_descriptor = next_ptr;

        /* Set previous entry's sze to include this size */
        prev_ptr->entry_size = (UNS_32) next_ptr -
                               (UNS_32) prev_ptr;

        /* Set next entry to point to new previous entry */
        next_ptr->prev_descriptor = prev_ptr;
      }
      else if ((prev_ptr->entry_size == 0) &&
               (next_ptr->entry_size != 0))
      {
        /* Previous entry used, next entry not used, so
           merge this entry and the next entry */

        /* Does an entry exist after the next entry? */
        saved_ptr = next_ptr->next_descriptor;
        if (saved_ptr == HEAP_POINTER_NULL)
        {
          /* No entry after the next entry, so this entry
             will become the last entry */
          found_ptr->entry_size = heap_size_saved -
                                  ((UNS_32) found_ptr - (UNS_32) heap_base);
          found_ptr->next_descriptor = HEAP_POINTER_NULL;
        }
        else
        {
          /* An entry exists after the next entry, so
             merge the two entries and update links */
          found_ptr->entry_size =
            (UNS_32) saved_ptr - (UNS_32) found_ptr;

          /* Update next descriptor */
          found_ptr->next_descriptor = saved_ptr;

          /* Reset previous descriptor link */
          saved_ptr->prev_descriptor = found_ptr;
        }
      }
      else
      {
        /* Previous and next entry are both not used, so
           all three entries have to be merged */

        /* Does an entry exist after the next entry? */
        saved_ptr = next_ptr->next_descriptor;
        if (saved_ptr == HEAP_POINTER_NULL)
        {
          /* No entry after the next entry, so this entry
             will become the last entry */
          prev_ptr->entry_size = heap_size_saved -
                                 ((UNS_32) prev_ptr - (UNS_32) heap_base);
          prev_ptr->next_descriptor = HEAP_POINTER_NULL;
        }
        else
        {
          /* An entry exists after the next entry, so
             merge all three entries and update links */
          prev_ptr->entry_size = (UNS_32) saved_ptr -
                                 (UNS_32) prev_ptr;

          /* Update next and previous links */
          prev_ptr->next_descriptor = saved_ptr;
          saved_ptr->prev_descriptor = prev_ptr;
        }
      }
    }

    /* Return a good status */
    status = 1;
  }

  return status;
}

/***********************************************************************
 * Public functions
 **********************************************************************/

/***********************************************************************
 *
 * Function: lpc_get_heapsize
 *
 * Purpose: Returns the size of the heap.
 *
 * Processing:
 *     See function.
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: The size of the heap area in bytes.
 *
 * Notes: None
 *
 **********************************************************************/
UNS_32 lpc_get_heapsize(void)
{
  return heap_size_saved;
}

/***********************************************************************
 *
 * Function: lpc_get_largest_chunk
 *
 * Purpose:
 *     Returns the largest available chunk in the heap.
 *
 * Processing:
 *     This function traverses through the heap list. If an entry has an
 *     available size of greater than 0 bytes, then the entry is assumed
 *     as free and the size of the chunk is compared to the running size
 *     count. If the size is larger, the running size count is updated
 *     with the new size.
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: The size of the largest chunk available in the heap area
 *          in bytes.
 *
 * Notes: None
 *
 **********************************************************************/
UNS_32 lpc_get_largest_chunk(void)
{
  HEAP_DESCRIPTOR_T *heap_ptr;
  UNS_32 max_chunk_size = 0;

  /* Start at top of heap list */
  heap_ptr = heap_base;

  /* Go through all the entries */
  while (heap_ptr != HEAP_POINTER_NULL)
  {
    if (heap_ptr->entry_size > max_chunk_size)
    {
      /* This chunk is larger than the last saved chunk size */
      max_chunk_size = heap_ptr->entry_size;
    }

    /* Next heap entry */
    heap_ptr = heap_ptr->next_descriptor;
  }

  /* Largest available chunk size is adjusted by the required space
     needed for the heap descriptor */
  return max_chunk_size;
}

/***********************************************************************
 *
 * Function: lpc_get_allocated_count
 *
 * Purpose:
 *     Return the number of allocated items in the heap.
 *
 * Processing:
 *     This function traverses through the heap list. If an entry has
 *     an available size of 0 bytes, then the entry is assumed as
 *     allocated and the allocated count is incremented.
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: The number of allocated heap entries.
 *
 * Notes: None
 *
 **********************************************************************/
UNS_32 lpc_get_allocated_count(void)
{
  HEAP_DESCRIPTOR_T *heap_ptr;
  UNS_32 heap_entries = 0;

  /* Start at top of heap list */
  heap_ptr = heap_base;

  /* Go through all the entries */
  while (heap_ptr != HEAP_POINTER_NULL)
  {
    if (heap_ptr->entry_size == 0)
    {
      /* This is an used chunk */
      heap_entries++;
    }

    /* Next heap entry */
    heap_ptr = heap_ptr->next_descriptor;
  }

  /* Largest available chunk size is adjusted by the required space
     needed for the heap descriptor */
  return heap_entries;
}

/***********************************************************************
 *
 * Function: lpc_get_heap_base
 *
 * Purpose: Return the heap base address.
 *
 * Processing:
 *     See function.
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: The base address of where heap memory starts.
 *
 * Notes: None
 *
 **********************************************************************/
void *lpc_get_heap_base(void)
{
  return heap_base;
}

/***********************************************************************
 *
 * Function: lpc_heap_init
 *
 * Purpose: Setup the heap area.
 *
 * Processing:
 *     The heap base address and size counters are set with the passed
 *     parameter values. The first entry of the heap is set up with an
 *     unallocated heap list entry.
 *
 * Parameters:
 *     base_addr : Base address of where heap starts
 *     heap_size : Size of heap area in bytes
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
void lpc_heap_init(void *base_addr, UNS_32 heap_size)
{
  HEAP_DESCRIPTOR_T *heap_ptr;

  /* Save heap base address and size */
  heap_base = (HEAP_DESCRIPTOR_T *) base_addr;
  heap_size_saved = heap_size;

  /* Setup first link in heap list */
  heap_ptr = (HEAP_DESCRIPTOR_T *) base_addr;

  /* Setup next heap entry pointer */
  heap_ptr->next_descriptor = HEAP_POINTER_NULL;

  /* Previous entry just points to itself (end of list) */
  heap_ptr->prev_descriptor = base_addr;

  /* Size of heap area (not including the descriptor) */
  heap_ptr->entry_size = heap_size;
}

/***********************************************************************
 *
 * Function: lpc_new
 *
 * Purpose: Get an allocated area from the heap.
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *     size_in_bytes : Byte size of the requested allocation chunk
 *
 * Outputs: None
 *
 * Returns: A pointer to the allocated chunk, or '0' if no room is
 *          available.
 *
 * Notes: None
 *
 **********************************************************************/
void *lpc_new(UNS_32 size_in_bytes)
{
  return (void *) lpc_heap_insert_entry(size_in_bytes);
}

/***********************************************************************
 *
 * Function: lpc_free
 *
 * Purpose: Returns an allocated entry of memory to the heap.
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *     free_addr : Address of allocated entry to return to heap
 *
 * Outputs: None
 *
 * Returns: '1' if the entry was deleted, otherwise '0'.
 *
 * Notes: None
 *
 **********************************************************************/
INT_32 lpc_free(void *free_addr)
{
  return lpc_remove_entry(free_addr);
}
