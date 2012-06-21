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

#include <alloca.h>

#include "json_value.h"

__PUBLIC__ json_memory_t stdlib_memory = {
     malloc, free
};

static void kill_object(json_visitor_t *this, json_object_t *visited) {
     int i;
     int n = visited->count(visited);
     const char **keys = (const char **)alloca(n * sizeof(const char*));
     visited->keys(visited, keys);
     for (i = 0; i < n; i++) {
          json_value_t *v = visited->get(visited, keys[i]);
          visited->del(visited, keys[i]);
          v->accept(v, this);
     }
     visited->free(visited);
}

static void kill_array(json_visitor_t *this, json_array_t  *visited) {
     int n = visited->count(visited);
     int i;
     for (i = n; i --> 0; ) {
          json_value_t *v = visited->get(visited, i);
          v->accept(v, this);
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
     0, /* don't del this singleton */
     (json_visit_object_fn)&kill_object,
     (json_visit_array_fn )&kill_array ,
     (json_visit_string_fn)&kill_string,
     (json_visit_number_fn)&kill_number,
     (json_visit_const_fn )&kill_const ,
};

__PUBLIC__ json_visitor_t *json_kill() {
     return &json_killer;
}
