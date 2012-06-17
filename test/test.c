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

#include "json.h"

int main() {
     json_object_t *value = json_new_object();
     json_value_t *v;
     json_value_t *t = (json_value_t*)json_const(json_true);
     value->set_value(value, "foo", t);
     if (strcmp("foo", value->get_field(value, 0).key)) return 1;
     if (value->get_field(value, 0).value != t) return 2;
     v = value->get_value(value, "foo");
     if (v != t) return 3;
     return 0;
}
