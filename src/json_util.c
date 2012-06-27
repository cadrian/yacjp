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
 * @ingroup json_utils
 * @file
 *
 * This file contains the implementation of utilities.
 */

#include <alloca.h>
#include <stdarg.h>

#include "json.h"

__PUBLIC__ json_memory_t stdlib_memory = {
     malloc, free
};

static void dont_free_visitor(json_visitor_t *this) {
     /* nothing */
}

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
     (json_visit_free_fn  )&dont_free_visitor,
     (json_visit_object_fn)&kill_object,
     (json_visit_array_fn )&kill_array ,
     (json_visit_string_fn)&kill_string,
     (json_visit_number_fn)&kill_number,
     (json_visit_const_fn )&kill_const ,
};

__PUBLIC__ json_visitor_t *json_kill() {
     return &json_killer;
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

__PUBLIC__ json_stop_t json_stop() {
     json_stop_t result = {
          .index = -1,
     };
     return result;
}

typedef struct lookup_visitor {
     json_visitor_t fn;
     json_value_t *result;
     va_list arg;
} lookup_visitor_t;

static void lookup_object(lookup_visitor_t *this, json_object_t *visited) {
     char *key = va_arg(this->arg, char*);
     if (key == JSON_STOP.key) {
          this->result = (json_value_t*)visited;
     }
     else {
          json_value_t *value = visited->get(visited, key);
          if (value == NULL) {
             this->result = NULL;
          }
          else {
             value->accept(value, (json_visitor_t*)this);
          }
     }
}

static void lookup_array(lookup_visitor_t *this, json_array_t  *visited) {
     int index = va_arg(this->arg, int);
     if (index == JSON_STOP.index) {
          this->result = (json_value_t*)visited;
     }
     else {
          json_value_t *value = visited->get(visited, index);
          if (value == NULL) {
             this->result = NULL;
          }
          else {
             value->accept(value, (json_visitor_t*)this);
          }
     }
}

static void lookup_string(lookup_visitor_t *this, json_string_t *visited) {
     this->result = (json_value_t*)visited;
}

static void lookup_number(lookup_visitor_t *this, json_number_t *visited) {
     this->result = (json_value_t*)visited;
}

static void lookup_const(lookup_visitor_t *this, json_const_t  *visited) {
     this->result = (json_value_t*)visited;
}

__PUBLIC__ json_value_t *json_lookup(json_value_t *value, ...) {
     lookup_visitor_t visitor = {
          {
               (json_visit_free_fn  )&dont_free_visitor,
               (json_visit_object_fn)&lookup_object,
               (json_visit_array_fn )&lookup_array ,
               (json_visit_string_fn)&lookup_string,
               (json_visit_number_fn)&lookup_number,
               (json_visit_const_fn )&lookup_const ,
          },
          NULL,
     };
     va_start(visitor.arg, value);
     value->accept(value, &(visitor.fn));
     va_end(visitor.arg);
     return visitor.result;
}
