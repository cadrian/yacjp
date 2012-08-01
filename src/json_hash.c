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

/**
 * @ingroup json_hash
 * @file
 *
 * This file contains the implementation of hash tables. That
 * implementation is a general-purpose hashing table independant of
 * any JSON consideration.
 *
 * The original hashing algorithm comes from Python; implementation
 * and bugs are mine.
 */

#include <string.h>

#include "hash.h"

#define PERTURB_SHIFT 5

typedef struct json_hash_key {
     const void *key;
     unsigned int hash;
} json_hash_key_t;

typedef struct json_hash_entry {
     json_hash_key_t  key;
     void            *value;
} json_hash_entry_t;

struct json_hash_impl {
     hash_t fn;
     json_memory_t memory;
     hash_keys_t   keys;

     int capacity;
     int count;
     json_hash_entry_t *entries;
};

static unsigned int string_hash(const char *key) {
     unsigned int result = 0;
     while (*key) {
          result = result * 13 + *key;
          key++;
     }
     return result;
}

__PUBLIC__ hash_keys_t hash_strings = {
     (hash_keys_hash_fn)string_hash,
     (hash_keys_compare_fn)strcmp,
     (hash_keys_clone_fn)strdup,
     (hash_keys_free_fn)free,
};

static json_hash_key_t hash(const char *key, hash_keys_t keys) {
     json_hash_key_t result = { key, keys.hash(key) };
     return result;
}

typedef struct index_context {
     unsigned int index;
     unsigned int perturb;
} index_context_t;

static void next_index_of(index_context_t *context) {
     context->index = (5 * context->index) + 1 + context->perturb;
     context->perturb >>= PERTURB_SHIFT;
}

static int index_of(json_hash_entry_t *entries, hash_keys_t keys, int capacity, json_hash_key_t key, index_context_t *save_context) {
     int result;
     const void *k;
     int found;
     hash_keys_compare_fn cmp = keys.compare;

     index_context_t context = {
          key.hash,
          key.hash,
     };

     result = context.index % capacity;
     k = entries[result].key.key;
     found = k ? !cmp(key.key, k) : 1;

     while (!found) {
          next_index_of(&context);
          result = context.index % capacity;
          k = entries[result].key.key;
          found = k ? !cmp(key.key, k) : 1;
     }

     if (!k) result = -result - 1;

     if (save_context) *save_context = context;

     return result;
}

static void rehash(struct json_hash_impl *this) {
     int i, index;
     json_hash_key_t key;

     for (i = 0; i < this->capacity; i++) {
          key = this->entries[i].key;
          if (key.key) {
               index = index_of(this->entries, this->keys, this->capacity, key, NULL);
               if (index < 0) {
                    // broken collision cycle, fix it
                    index = -index - 1;
                    this->entries[index].key   = key;
                    this->entries[index].value = this->entries[i].value;
                    this->entries[i].key.key = NULL;
                    this->entries[i].value   = NULL;
                    i = -1; // restart
               }
          }
     }
}

static void grow(struct json_hash_impl *this, int grow_factor) {
     json_hash_entry_t *new_entries;
     int new_capacity;
     json_hash_entry_t field;
     int i, index;
     if (this->capacity == 0) {
          new_capacity = 4;
          new_entries = (json_hash_entry_t *)this->memory.malloc(new_capacity * sizeof(json_hash_entry_t));
          memset(new_entries, 0, new_capacity * sizeof(json_hash_entry_t));
     }
     else {
          new_capacity = this->capacity * grow_factor;
          new_entries = (json_hash_entry_t *)this->memory.malloc(new_capacity * sizeof(json_hash_entry_t));
          memset(new_entries, 0, new_capacity * sizeof(json_hash_entry_t));
          for (i = 0; i < this->capacity; i++) {
               field = this->entries[i];
               if (field.key.key) {
                    index = -index_of(new_entries, this->keys, new_capacity, field.key, NULL) - 1;
                    new_entries[index] = field;
               }
          }
          this->memory.free(this->entries);
     }
     this->capacity = new_capacity;
     this->entries = new_entries;
}

static unsigned int count(struct json_hash_impl *this) {
     return this->count;
}

static void iterate(struct json_hash_impl *this, hash_iterator_fn iterator, void *data) {
     int i, index = 0;
     json_hash_entry_t entry;
     for (i = 0; index < this->count; i++) {
          entry = this->entries[i];
          if (entry.key.key) {
               iterator(this, index++, entry.key.key, entry.value, data);
          }
     }
}

static void *get(struct json_hash_impl *this, const void *key) {
     void *result = NULL;
     int index;
     if (this->capacity) {
          index = index_of(this->entries, this->keys, this->capacity, hash(key, this->keys), NULL);
          if (index >= 0) {
               result = this->entries[index].value;
          }
     }
     return result;
}

static void *set(struct json_hash_impl *this, const void *key, void *value) {
     void *result = NULL;
     int index;
     json_hash_key_t hkey = hash(key, this->keys);
     if (this->capacity == 0) {
          grow(this, 2);
     }
     index = index_of(this->entries, this->keys, this->capacity, hkey, NULL);
     if (index >= 0) {
          result = this->entries[index].value;
     }
     else {
          if (this->count * 3 >= this->capacity * 2) {
               grow(this, 2);
               index = index_of(this->entries, this->keys, this->capacity, hkey, NULL);
          }
          index = -index - 1;
          hkey.key = this->keys.clone(key);
          this->entries[index].key = hkey;
          this->count++;
     }
     this->entries[index].value = value;
     return result;
}

static void *del(struct json_hash_impl *this, const void *key) {
     void *result = NULL;
     json_hash_key_t hkey = hash(key, this->keys);
     int index = index_of(this->entries, this->keys, this->capacity, hkey, NULL);

     if (index >= 0) {
          result = this->entries[index].value;
          this->keys.free(this->entries[index].key.key);
          this->entries[index].key.key = NULL;
          this->entries[index].value   = NULL;
          this->count--;
          rehash(this);
     }
     return result;
}

static void free_(struct json_hash_impl *this) {
     if (this->entries) this->memory.free(this->entries);
     this->memory.free(this);
}

static hash_t fn = {
     (hash_free_fn   )free_  ,
     (hash_count_fn  )count  ,
     (hash_iterate_fn)iterate,
     (hash_get_fn    )get    ,
     (hash_set_fn    )set    ,
     (hash_del_fn    )del    ,
};

__PUBLIC__ hash_t *new_hash(json_memory_t memory, hash_keys_t keys) {
     struct json_hash_impl *result = (struct json_hash_impl *)memory.malloc(sizeof(struct json_hash_impl));
     if (!result) return NULL;
     result->fn      = fn;
     result->memory  = memory;
     result->keys    = keys;
     result->capacity= 0;
     result->count   = 0;
     result->entries = NULL;
     return (hash_t*)result;
}
