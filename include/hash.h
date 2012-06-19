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

#ifndef _YACJP_HASH_H_
#define _YACJP_HASH_H_

#include "json_shared.h"

typedef struct hash hash_t;

typedef void (*hash_iterator_fn)(void *hash, int index, const void *key, void *value, void *data);

typedef unsigned int (*hash_count_fn)  (hash_t *this);
typedef void         (*hash_free_fn)   (hash_t *this);
typedef void         (*hash_iterate_fn)(hash_t *this, hash_iterator_fn iterator, void *data);
typedef void        *(*hash_get_fn)    (hash_t *this, const void *key);
typedef void        *(*hash_set_fn)    (hash_t *this, const void *key, void *value);
typedef void        *(*hash_del_fn)    (hash_t *this, const void *key);

struct hash {
     hash_free_fn    free;
     hash_count_fn   count;
     hash_iterate_fn iterate;
     hash_get_fn     get;
     hash_set_fn     set;
     hash_del_fn     del;
};

typedef unsigned int (*hash_fn)   (const void *key);
typedef int          (*compare_fn)(const void *key1, const void *key2);
typedef const void  *(*clone_fn)  (const void *key);
typedef void         (*free_fn)   (const void *key);

typedef struct hash_keys {
     hash_fn    hash;
     compare_fn compare;
     clone_fn   clone;
     free_fn    free;
} hash_keys_t;

__PUBLIC__ extern hash_keys_t hash_strings;

__PUBLIC__ hash_t *new_hash(json_memory_t memory, hash_keys_t keys);

#endif /* _YACJP_HASH_H_ */
