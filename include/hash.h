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
 * @ingroup json_hash
 * @file
 *
 * A hash table. Accepts any kinds of pointers as keys and values (you
 * must provide the functions to hash the keys).
 *
 * The implementation is based on Python's.
 */

#include "json_shared.h"

/**
 * @addtogroup json_hash
 * @{
 */

/**
 * The hash table public interface.
 */
typedef struct hash hash_t;

/**
 * The user must provide a function of this type to iterate through
 * all the keys of a hash table.
 *
 * @param[in] hash the hash table onto which the iterator is iterating
 * @param[in] index the current index; the function is called once for each [0..count[
 * @param[in] key the current key
 * @param[in] value the current value
 * @param[in] data user data
 *
 */
typedef void (*hash_iterator_fn)(void *hash, int index, const void *key, void *value, void *data);

/**
 * Frees the hash table.
 *
 * \a Note: does not free its content!
 *
 * @param[in] this the target hash table
 *
 */
typedef void         (*hash_free_fn)   (hash_t *this);

/**
 * Counts the number of elements in the hash table.
 *
 * @param[in] this the target hash table
 *
 * @return the number of elements.
 *
 */
typedef unsigned int (*hash_count_fn)  (hash_t *this);

/**
 * Iterates through all the hash table's keys. Calls the provided
 * `iterator` for each key.
 *
 * @param[in] this the target hash table
 * @param[in] iterator the function called once per key
 * @param[in] data a user data pointer passed to the `iterator` function
 *
 */
typedef void         (*hash_iterate_fn)(hash_t *this, hash_iterator_fn iterator, void *data);

/**
 * Retrieves a value associated with the given `key`.
 *
 * @param[in] this the target hash table
 * @param[in] key the key to lookup
 *
 * @return the value associated to the provided key, `NULL` if not found.
 *
 */
typedef void        *(*hash_get_fn)    (hash_t *this, const void *key);

/**
 * Associates a `value` to a `key`. If not already set, the `key` is
 * cloned (otherwise the internal already cloned key is used). The
 * provided `key` may be freed by the caller. On the other hand the
 * `value` is stored as is.
 *
 * @param[in] this the target hash table
 * @param[in] key the key to set
 * @param[in] value the value to associate to the `key`
 *
 * @return the previous value, or NULL
 *
 */
typedef void        *(*hash_set_fn)    (hash_t *this, const void *key, void *value);

/**
 * Removes both a `key` and its associated value from the hash table.
 *
 * @param[in] this the target hash table
 * @param[in] key the key to delete
 *
 * @return the previous value, or NULL
 *
 */
typedef void        *(*hash_del_fn)    (hash_t *this, const void *key);

struct hash {
     /**
      * @see hash_free_fn
      */
     hash_free_fn    free;
     /**
      * @see hash_count_fn
      */
     hash_count_fn   count;
     /**
      * @see hash_iterate_fn
      */
     hash_iterate_fn iterate;
     /**
      * @see hash_get_fn
      */
     hash_get_fn     get;
     /**
      * @see hash_set_fn
      */
     hash_set_fn     set;
     /**
      * @see hash_del_fn
      */
     hash_del_fn     del;
};

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/**
 * The user must provide a function of this type to hash the given
 * `key`.
 *
 * @param[in] key the key to hash
 *
 * @return the hash value of a key
 */
typedef unsigned int (*hash_keys_hash_fn)   (const void *key);

/**
 * The user must provide a function that compares both given keys.
 *
 * @param[in] key1 the first key
 * @param[in] key2 the second key
 *
 * @post given `hash` as a @ref hash_keys_hash_fn, `(result == 0) => (hash(key1) == hash(key2))`
 *
 * @return 0 if both keys are equal, non-zero otherwise.
 */
typedef int          (*hash_keys_compare_fn)(const void *key1, const void *key2);

/**
 * Clone the key to keep an private one in the hash table.
 *
 * @return a newly allocated key (to be kept in the hash table);
 * the result must be equal to the provided key.
 *
 * @post given `compare` as a @ref hash_keys_compare_fn, `compare(result, key) == 0`
 *
 */
typedef const void  *(*hash_keys_clone_fn)  (const void *key);

/**
 * Frees the given `key` which is guaranteed to have been clone()d
 * by the hash table. Used at del() time.
 */
typedef void         (*hash_keys_free_fn)   (const void *key);

/**
 * The user must provide an object with this public interface of the
 * functions to the hash table creator (new_hash()). The hash table
 * will use it to manage its internal keys.
 */
typedef struct hash_keys {
     /**
      * @see hash_keys_hash_fn
      */
     hash_keys_hash_fn    hash;
     /**
      * @see hash_keys_compare_fn
      */
     hash_keys_compare_fn compare;
     /**
      * @see hash_keys_clone_fn
      */
     hash_keys_clone_fn   clone;
     /**
      * @see hash_keys_free_fn
      */
     hash_keys_free_fn    free;
} hash_keys_t;

/**
 * A classic keys manager when keys are C strings (`char*`).
 */
__PUBLIC__ extern hash_keys_t hash_strings;

/**
 * Allocates and returns a new hash table.
 *
 * @return the newly allocated hash table.
 */
__PUBLIC__ hash_t *new_hash(json_memory_t memory, hash_keys_t keys);

/**
 * @}
 */

#endif /* _YACJP_HASH_H_ */
