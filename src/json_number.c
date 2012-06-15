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

#include "json_value.h"

struct json_number_impl {
   struct json_number fn;

   int integral;
   int decimal;
   int decimal_exp;
   int exponent;
}

static void accept(struct json_number_impl *this, json_visitor_t *visitor) {
   visitor->visit_number(visitor, (json_number_t*)this);
}

static int is_int(struct json_number_impl *this) {
   return this->decimal_exp <= this->exponent;
}

static int to_int(struct json_number_impl *this) {
   int sign = this->integral;
   if (sign) sign = sign > 0 ? 1 : -1;
   return this->integral * 10 ^ this->exponent
      + sign * this->decimal * 10 ^ (this->exponent - this->decimal_exp);
}

static double to_double(struct json_number_impl *this) {
   int sign = this->integral;
   if (sign) sign = sign > 0 ? 1 : -1;
   return (double)this->integral * 10.0 ^ this->exponent
      + sign * (double)this->decimal / 10.0 ^ (this->decimal_exp - this->exponent);
}

static void set(struct json_number_impl *this, int i, int d, int dx, int x) {
   this->integral = i;
   this->decimal = d;
   this->decimal_exp = dx;
   this->exponent = x;
}

__PUBLIC__ json_value_t *json_new_number() {
   struct json_number_impl *result = (struct json_number_impl *)malloc(sizeof(json_number_t *));
   result->fn.accept    = (json_number_accept_fn   )accept;
   result->fn.is_int    = (json_number_is_int_fn   )0;
   result->fn.to_int    = (json_number_to_int_fn   )0;
   result->fn.to_double = (json_number_to_double_fn)0;
   result->fn.set       = (json_number_set_fn      )0;
   set(result, 0, 0, 0, 0);
   return (json_value_t*)result;
}
