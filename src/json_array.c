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

#include <string.h>

#include "json_value.h"

struct json_array_impl {
     struct json_array fn;
     json_memory_t memory;

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

static json_value_t *get(struct json_array_impl *this, int index) {
     json_value_t *result = NULL;
     if (index >= 0 && index < this->count) {
          result = this->values[index];
     }
     return result;
}

static void set(struct json_array_impl *this, int index, json_value_t *value) {
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

static void ins(struct json_array_impl *this, int index, json_value_t *value) {
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

static void del(struct json_array_impl *this, int index) {
     if (index >= 0 && index < this->count) {
          memmove(this->values + index, this->values + index + 1, this->count - index - 1);
          this->count--;
     }
}

static void free_(struct json_array_impl *this) {
     if (this->values) this->memory.free(this->values);
     this->memory.free(this);
}

__PUBLIC__ json_array_t *json_new_array(json_memory_t memory) {
     struct json_array_impl *result = (struct json_array_impl *)memory.malloc(sizeof(struct json_array_impl));
     if (!result) return NULL;
     result->fn.accept    = (json_array_accept_fn)accept;
     result->fn.free      = (json_array_free_fn  )free_ ;
     result->fn.count     = (json_array_count_fn )count ;
     result->fn.get       = (json_array_get_fn   )get   ;
     result->fn.set       = (json_array_set_fn   )set   ;
     result->fn.ins       = (json_array_set_fn   )ins   ;
     result->fn.add       = (json_array_add_fn   )add   ;
     result->fn.del       = (json_array_del_fn   )del   ;
     result->memory       = memory;
     result->capacity     = 0;
     result->count        = 0;
     result->values       = NULL;
     return &(result->fn);
}
