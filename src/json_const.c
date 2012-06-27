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
 * @ingroup json_const
 * @file
 *
 * This file contains the implementation of JSON constants.
 */

#include <string.h>

#include "json_value.h"

struct json_const_impl {
     struct json_const fn;

     json_const_e value;
};

static void accept(struct json_const_impl *this, json_visitor_t *visitor) {
     visitor->visit_const(visitor, (json_const_t*)this);
}

static json_const_e value(struct json_const_impl *this) {
     return this->value;
}

static void free_(struct json_const_impl *this) {
     /* nothing */
}

struct json_const_impl _json_const[] = {
     {{(json_const_accept_fn)accept, (json_const_free_fn)free_, (json_const_value_fn )value}, json_false},
     {{(json_const_accept_fn)accept, (json_const_free_fn)free_, (json_const_value_fn )value}, json_true },
     {{(json_const_accept_fn)accept, (json_const_free_fn)free_, (json_const_value_fn )value}, json_null },
};

__PUBLIC__ json_const_t *json_const(json_const_e value) {
     return (json_const_t*)&_json_const[value];
}
