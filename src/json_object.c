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

#include <string.h>

#include "json_value.h"
#include "hash.h"

struct json_object_impl {
     struct json_object fn;
     json_memory_t memory;

     hash_t *hash;
};

static void accept(struct json_object_impl *this, json_visitor_t *visitor) {
     visitor->visit_object(visitor, (json_object_t*)this);
}

static unsigned int count(struct json_object_impl *this) {
     return this->hash->count(this->hash);
}

static void keys_iterator(struct json_object_impl *this, int index, const char *key, json_value_t *value, const char **keys) {
     keys[index] = key;
}

static void keys(struct json_object_impl *this, const char **keys) {
     this->hash->iterate(this->hash, (hash_iterator_fn)keys_iterator, keys);
}

static json_value_t *get(struct json_object_impl *this, const char *key) {
     return (json_value_t*)this->hash->get(this->hash, key);
}

static json_value_t *set(struct json_object_impl *this, const char *key, json_value_t *value) {
     return (json_value_t*)this->hash->set(this->hash, key, value);
}

static json_value_t *del(struct json_object_impl *this, const char *key) {
     return (json_value_t*)this->hash->del(this->hash, key);
}

static void free_(struct json_object_impl *this) {
     this->hash->free(this->hash);
     this->memory.free(this);
}

__PUBLIC__ json_object_t *json_new_object(json_memory_t memory) {
     struct json_object_impl *result = (struct json_object_impl *)memory.malloc(sizeof(struct json_object_impl));
     if (!result) return NULL;
     result->fn.accept = (json_object_accept_fn)accept ;
     result->fn.free   = (json_object_free_fn  )free_  ;
     result->fn.count  = (json_object_count_fn )count  ;
     result->fn.keys   = (json_object_keys_fn  )keys   ;
     result->fn.get    = (json_object_get_fn   )get    ;
     result->fn.set    = (json_object_set_fn   )set    ;
     result->fn.del    = (json_object_del_fn   )del    ;
     result->memory    = memory                        ;
     result->hash      = new_hash(memory, hash_strings);
     return (json_object_t*)result;
}
