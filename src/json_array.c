/*
  This file is part of YacJP.

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
 * @ingroup json_array
 * @file
 *
 * This file contains the implementation of JSON arrays.
 */

#include <string.h>

#include "json_value.h"

struct json_array_impl {
     struct json_array fn;
     cad_memory_t memory;

     int capacity;
     int count;
     json_value_t **values;
};

static void accept(struct json_array_impl *this, json_visitor_t *visitor) {
     visitor->visit_array(visitor, (json_array_t*)this);
}

static void grow(struct json_array_impl *this) {
     int new_capacity;
     json_value_t **new_values;
     if (this->capacity == 0) {
          new_capacity = 4;
          new_values = (json_value_t **)this->memory.malloc(new_capacity * sizeof(json_value_t*));
     }
     else {
          new_capacity = this->capacity * 2;
          new_values = (json_value_t **)this->memory.malloc(new_capacity * sizeof(json_value_t*));
          memset(new_values + this->capacity, 0, this->capacity * sizeof(json_value_t*));
          memcpy(new_values, this->values, this->capacity * sizeof(json_value_t*));
          this->memory.free(this->values);
     }
     this->capacity = new_capacity;
     this->values = new_values;
}

static unsigned int count(struct json_array_impl *this) {
     return this->count;
}

static json_value_t *get(struct json_array_impl *this, unsigned int index) {
     json_value_t *result = NULL;
     if (index >= 0 && index < this->count) {
          result = this->values[index];
     }
     return result;
}

static void set(struct json_array_impl *this, unsigned int index, json_value_t *value) {
     if (index < this->count) {
          this->values[index] = value;
     }
     else {
          while (index >= this->capacity) {
               grow(this);
          }
          this->values[index] = value;
          this->count = index + 1;
     }
}

static void ins(struct json_array_impl *this, unsigned int index, json_value_t *value) {
     if (index >= this->count) {
          set(this, index, value);
     }
     else {
          if (this->count == this->capacity) {
               grow(this);
          }
          while (index > this->capacity) {
               grow(this);
          }
          memmove(this->values + index + 1, this->values + index, this->count - index);
          this->values[index] = value;
          this->count++;
     }
}

static void add(struct json_array_impl *this, json_value_t *value) {
     set(this, this->count, value);
}

static void del(struct json_array_impl *this, unsigned int index) {
     if (index >= 0 && index < this->count) {
          memmove(this->values + index, this->values + index + 1, this->count - index - 1);
          this->count--;
     }
}

static void free_(struct json_array_impl *this) {
     if (this->values) this->memory.free(this->values);
     this->memory.free(this);
}

static json_array_t fn = {
     (json_array_accept_fn)accept,
     (json_array_free_fn  )free_ ,
     (json_array_count_fn )count ,
     (json_array_get_fn   )get   ,
     (json_array_set_fn   )set   ,
     (json_array_set_fn   )ins   ,
     (json_array_add_fn   )add   ,
     (json_array_del_fn   )del   ,
};

__PUBLIC__ json_array_t *json_new_array(cad_memory_t memory) {
     struct json_array_impl *result = (struct json_array_impl *)memory.malloc(sizeof(struct json_array_impl));
     if (!result) return NULL;
     result->fn       = fn;
     result->memory   = memory;
     result->capacity = 0;
     result->count    = 0;
     result->values   = NULL;
     return &(result->fn);
}
