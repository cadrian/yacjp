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

#include <stdlib.h>

#define __PUBLIC__ __attribute__((__visibility__("default")))

typedef void *(*json_malloc_fn)(size_t);
typedef void  (*json_free_fn)(void*);

typedef struct json_memory {
     json_malloc_fn malloc;
     json_free_fn   free;
} json_memory_t;

__PUBLIC__ extern json_memory_t stdlib_memory;

#endif /* _YACJP_JSON_SHARED_H_ */
