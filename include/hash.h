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

/**
 *
 * A hash table. Accepts any kinds of pointers (you must provide the
 * functions to hash the keys).
 *
 * The implementation is based on Python's.
 *
 */

#include "json_shared.h"

typedef struct hash hash_t;

/**
 * Provide a function of this type to iterate through all the keys of
 * a hash table.
 */
typedef void (*hash_iterator_fn)(void *hash, int index, const void *key, void *value, void *data);

typedef unsigned int (*hash_count_fn)  (hash_t *this);
typedef void         (*hash_free_fn)   (hash_t *this);
typedef void         (*hash_iterate_fn)(hash_t *this, hash_iterator_fn iterator, void *data);
typedef void        *(*hash_get_fn)    (hash_t *this, const void *key);
typedef void        *(*hash_set_fn)    (hash_t *this, const void *key, void *value);
typedef void        *(*hash_del_fn)    (hash_t *this, const void *key);

/**
 * The hash table public interface
 */
struct hash {
     /**
      * Free the hash table.
      *
      * Note: does not free its content!
      */
     hash_free_fn    free;

     /**
      * Return the number of elements.
      */
     hash_count_fn   count;

     /**
      * Iterates through all the hash table's keys. Calls the provided
      * iterator for each key.
      */
     hash_iterate_fn iterate;

     /**
      * Returns the value associated to the provided key.
      */
     hash_get_fn     get;

     /**
      * Associates a key and a value. The key is cloned.
      */
     hash_set_fn     set;

     /**
      * Removes both a key and its associated value from the hash
      * table.
      */
     hash_del_fn     del;
};

typedef unsigned int (*hash_fn)   (const void *key);
typedef int          (*compare_fn)(const void *key1, const void *key2);
typedef const void  *(*clone_fn)  (const void *key);
typedef void         (*free_fn)   (const void *key);

/**
 * Public interface of the functions to provide that will manage the
 * keys.
 */
typedef struct hash_keys {
     /**
      * @return the hash value of a key
      */
     hash_fn    hash;

     /**
      * @return 0 if both keys are equal, non-zero otherwise.
      */
     compare_fn compare;

     /**
      * @return a newly allocated key (to be kept in the hash table);
      * the result must be equal to the provided key (compare()==0)
      */
     clone_fn   clone;

     /**
      * Free the given key which is guaranteed to have been clone()d
      * by the hash table. Used at del() time.
      */
     free_fn    free;
} hash_keys_t;

/**
 * A classic keys manager when keys are C strings (char*).
 */
__PUBLIC__ extern hash_keys_t hash_strings;

/**
 * @return a newly allocated hash table.
 */
__PUBLIC__ hash_t *new_hash(json_memory_t memory, hash_keys_t keys);

#endif /* _YACJP_HASH_H_ */
