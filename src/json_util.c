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

#include "json_value.h"

static void kill_object(json_visitor_t *this, json_object_t *visited) {
     json_object_field_t field;
     while (visited->count(visited) > 0) {
          field = visited->get_field(visited, 0);
          visited->del_value(visited, field.key);
          if (field.value) {
               field.value->accept(field.value, this);
          }
     }
     visited->free(visited);
}

static void kill_array(json_visitor_t *this, json_array_t  *visited) {
     int n = visited->count(visited);
     int i;
     json_value_t *v;
     for (i = n; i --> 0; ) {
          v = visited->get(visited, i);
          if (v) {
               v->accept(v, this);
          }
     }
     visited->free(visited);
}

static void kill_string(json_visitor_t *this, json_string_t *visited) {
     visited->free(visited);
}

static void kill_number(json_visitor_t *this, json_number_t *visited) {
     visited->free(visited);
}

static void kill_const(json_visitor_t *this, json_const_t  *visited) {
     visited->free(visited);
}

json_visitor_t json_killer = {
     &kill_object,
     &kill_array ,
     &kill_string,
     &kill_number,
     &kill_const ,
};

__PUBLIC__ json_visitor_t *json_kill() {
     return &json_killer;
}
