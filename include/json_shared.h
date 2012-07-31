/*
  This file is part of YACJP.

  YacJP is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, version 3 of the License.

  YacJP is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with YacJP.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _YACJP_JSON_SHARED_H_
#define _YACJP_JSON_SHARED_H_

/**
 * @file
 * A shared header for all JSON subsystems.
 */

#include <stdlib.h>

#define __PUBLIC__ __attribute__((__visibility__("default")))
#define __PRINTF__ __attribute__((format(printf, 2, 3)))

/**
 * @addtogroup json_utils
 * @{
 */

/**
 * The memory allocator.
 * Works like `malloc(3)`.
 *
 * @param[in] size the size (in bytes) of the requested memory chunk to allocate.
 *
 * @return the newly allocated memory chunk, or `NULL` if it could not be allocated.
 */
typedef void *(*json_malloc_fn)(size_t size);

/**
 * The memory deallocator.
 * Works like `free(3)`.
 *
 * @param[in] the pointer to deallocate.
 */
typedef void  (*json_free_fn)(void *ptr);

/**
 * The user must provide an object providing this memory manager
 * interface to any function that may need to allocate and free chunks
 * of memory.
 */
typedef struct json_memory {
     /**
      * @see json_malloc_fn
      */
     json_malloc_fn malloc;

     /**
      * @see json_free_fn
      */
     json_free_fn   free;
} json_memory_t;

/**
 * Certainly the most used memory manager: the raw libc `malloc(3)`
 * and `free(3)` functions.
 */
__PUBLIC__ extern json_memory_t stdlib_memory;

/**
 * @}
 */

#endif /* _YACJP_JSON_SHARED_H_ */
