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
 * @ingroup json_number
 * @file
 *
 * This file contains the implementation of JSON numbers.
 */

#include <math.h>
#include <stdio.h>

#include "json_value.h"

struct json_number_impl {
     struct json_number fn;
     json_memory_t memory;

     int integral;
     int decimal;
     int decimal_exp;
     int exponent;
};

static void accept(struct json_number_impl *this, json_visitor_t *visitor) {
     visitor->visit_number(visitor, (json_number_t*)this);
}

static int is_int(struct json_number_impl *this) {
     return this->decimal_exp <= this->exponent;
}

static int to_int(struct json_number_impl *this) {
     int sign = this->integral;
     if (sign) sign = sign > 0 ? 1 : -1;
     return this->integral * pow(10, this->exponent)
          + sign * this->decimal * pow(10, (this->exponent - this->decimal_exp));
}

static double to_double(struct json_number_impl *this) {
     int sign = this->integral;
     if (sign) sign = sign > 0 ? 1 : -1;
     return (double)this->integral * pow(10, this->exponent)
          + sign * (double)this->decimal / pow(10, (this->decimal_exp - this->exponent));
}

static void set(struct json_number_impl *this, int i, int d, int dx, int x) {
     this->integral = i;
     this->decimal = d;
     this->decimal_exp = dx;
     this->exponent = x;
}

static int to_string(struct json_number_impl *this, char *buffer, size_t size) {
     if (this->decimal_exp == 0) {
          if (this->exponent == 0) {
               return snprintf(buffer, size, "%01d", this->integral);
          }
          else {
               return snprintf(buffer, size, "%01de%+d", this->integral, this->exponent);
          }
     }
     else {
          int n = this->decimal_exp - snprintf("", 0, "%d", this->decimal);
          if (this->exponent == 0) {
               return snprintf(buffer, size, "%d.%0*d", this->integral, n, this->decimal);
          }
          else {
               return snprintf(buffer, size, "%d.%0*de%+d", this->integral, n, this->decimal, this->exponent);
          }
     }
}

static void free_(struct json_number_impl *this) {
     this->memory.free(this);
}

__PUBLIC__ json_number_t *json_new_number(json_memory_t memory) {
     struct json_number_impl *result = (struct json_number_impl *)memory.malloc(sizeof(struct json_number_impl));
     if (!result) return NULL;
     result->fn.accept    = (json_number_accept_fn   )accept;
     result->fn.free      = (json_number_free_fn     )free_ ;
     result->fn.is_int    = (json_number_is_int_fn   )is_int;
     result->fn.to_int    = (json_number_to_int_fn   )to_int;
     result->fn.to_double = (json_number_to_double_fn)to_double;
     result->fn.set       = (json_number_set_fn      )set;
     result->fn.to_string = (json_number_to_string_fn)to_string;
     result->memory       = memory;
     set(result, 0, 0, 0, 0);
     return &(result->fn);
}
