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

#include <stdlib.h>
#include <string.h>

#include "json_value.h"

/* original hashing algorithm from Python, implementation and bugs are mine */

#define PERTURB_SHIFT 5

struct json_object_impl {
     struct json_object fn;

     int capacity;
     int count;
     json_object_field_t *fields;

     int last_index;
     int last_field;
};

static void accept(struct json_object_impl *this, json_visitor_t *visitor) {
     visitor->visit_object(visitor, (json_object_t*)this);
}

static int hash(const char *key) {
     int result = 0;
     while (*key) {
          result = result * 13 + *key;
          key++;
     }
     return result;
}

static int index_of(json_object_field_t *fields, int capacity, const char *key) {
     int h = hash(key);
     int j = h;
     int perturb = h;

     int result = j % capacity;
     const char *k = fields[result].key;
     int found = k ? !strcmp(key, k) : 1;
     do {
          j = (5 * j) + 1 + perturb;
          perturb >>= PERTURB_SHIFT;
          result = j % capacity;
          k = fields[result].key;
          found = k ? !strcmp(key, k) : 1;
     } while (!found);

     if (!k) result = -result - 1;

     return result;
}

static void grow(struct json_object_impl *this) {
     json_object_field_t *new_fields;
     int new_capacity;
     json_object_field_t field;
     int i, index;
     if (this->capacity = 0) {
          new_capacity = 4;
          new_fields = (json_object_field_t *)malloc(new_capacity * sizeof(json_object_field_t *));
          memset(new_fields, 0, new_capacity * sizeof(json_object_field_t *));
     }
     else {
          new_capacity = this->capacity * 2;
          new_fields = (json_object_field_t *)malloc(new_capacity * sizeof(json_object_field_t *));
          memset(new_fields, 0, new_capacity * sizeof(json_object_field_t *));
          for (i = 0; i < this->capacity; i++) {
               field = this->fields[i];
               if (field.key) {
                    index = -index_of(new_fields, new_capacity, field.key) - 1;
                    new_fields[index] = field;
               }
          }
     }
     this->capacity = new_capacity;
     this->fields = new_fields;
}

static unsigned int count(struct json_object_impl *this) {
     return this->count;
}

static json_object_field_t get_field(struct json_object_impl *this, int index) {
     json_object_field_t result = {0,};
     int i;
     if (this->last_index > index) {
          this->last_index = -1;
          this->last_field = -1;
     }
     i = this->last_field;
     while (this->last_index < index) {
          this->last_index++;
          do {
               i++;
          } while (i < this->capacity && !this->fields[i].key);
     }
     if (i < this->capacity) {
          result = this->fields[i];
     }
     return result;
}

static json_value_t *get_value(struct json_object_impl *this, const char *key) {
     json_value_t * result = NULL;
     int index = index_of(this->fields, this->capacity, key);
     if (index >= 0) {
          result = this->fields[index].value;
     }
     return result;
}

static void set_value(struct json_object_impl *this, const char *key, json_value_t *value) {
     int index = index_of(this->fields, this->capacity, key);
     if (index < 0) {
          if (this->count * 3 > this->capacity * 2) {
               grow(this);
               index = index_of(this->fields, this->capacity, key);
          }
          index = -index - 1;
          this->fields[index].key = key;
          this->count++;
     }
     this->fields[index].value = value;
}

static void del_value(struct json_object_impl *this, const char *key) {
     int index = index_of(this->fields, this->capacity, key);
     if (index >= 0) {
          this->fields[index].key   = NULL;
          this->fields[index].value = NULL;
          this->count--;
     }
}

__PUBLIC__ json_value_t *json_new_object() {
     struct json_object_impl *result = (struct json_object_impl *)malloc(sizeof(json_object_t *));
     if (!result) return NULL;
     result->fn.accept    = (json_object_accept_fn   )accept   ;
     result->fn.count     = (json_object_count_fn    )count    ;
     result->fn.get_field = (json_object_get_field_fn)get_field;
     result->fn.get_value = (json_object_get_value_fn)get_value;
     result->fn.set_value = (json_object_set_value_fn)set_value;
     result->fn.del_value = (json_object_del_value_fn)del_value;
     result->capacity     = 0;
     result->count        = 0;
     result->fields       = NULL;
     result->last_index = -1;
     result->last_field = -1;
     return (json_value_t*)result;
}
