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
 * This file contains the implementation of the clone visitor.
 */

#include "json.h"

struct json_visitor_clone {
     json_visitor_t fn;
     cad_memory_t memory;
     json_value_t **result;
};

static void visit_object(struct json_visitor_clone *this, json_object_t *visited) {
     int i, n = visited->count(visited);
     const char **keys = this->memory.malloc(n * sizeof(char*));
     json_object_t *clone = json_new_object(this->memory);
     json_value_t *value;
     visited->keys(visited, keys);
     for (i = 0; i < n; i++) {
          value = visited->get(visited, keys[i]);
          value->accept(value, &(this->fn));
          clone->set(clone, keys[i], *(this->result));
     }
     *(this->result) = (json_value_t*)clone;
     this->memory.free(keys);
}

static void visit_array(struct json_visitor_clone *this, json_array_t *visited) {
     int i, n = visited->count(visited);
     json_array_t *clone = json_new_array(this->memory);
     json_value_t *value;
     for (i = 0; i < n; i++) {
          value = visited->get(visited, i);
          value->accept(value, &(this->fn));
          clone->set(clone, i, *(this->result));
     }
     *(this->result) = (json_value_t*)clone;
}

static void visit_string(struct json_visitor_clone *this, json_string_t *visited) {
     json_string_t *clone = json_new_string(this->memory);
     size_t n = visited->utf8(visited, "", 0) + 1;
     char *buffer = this->memory.malloc(n);
     visited->utf8(visited, buffer, n);
     clone->add_string(clone, "%s", buffer);
     *(this->result) = (json_value_t*)clone;

     this->memory.free(buffer);
}

static void visit_number(struct json_visitor_clone *this, json_number_t *visited) {
     json_input_stream_t *str;
     size_t n = visited->to_string(visited, "", 0) + 1;
     char *buffer = this->memory.malloc(n);

     str = new_json_input_stream_from_string(buffer, this->memory);
     *(this->result) = json_parse(str, NULL, this->memory);

     this->memory.free(str);
     this->memory.free(buffer);
}

static void visit_const(struct json_visitor_clone *this, json_const_t *visited) {
     *(this->result) = (json_value_t*)visited;
}

static void free_(struct json_visitor_clone *this) {
     this->memory.free(this);
}

static json_visitor_t fn = {
     (json_visit_free_fn  ) free_,
     (json_visit_object_fn) visit_object,
     (json_visit_array_fn ) visit_array ,
     (json_visit_string_fn) visit_string,
     (json_visit_number_fn) visit_number,
     (json_visit_const_fn ) visit_const ,
};

__PUBLIC__ json_visitor_t *json_clone(json_value_t **res, cad_memory_t memory) {
     struct json_visitor_clone *result = malloc(sizeof(struct json_visitor_clone));
     result->fn = fn;
     result->memory = memory;
     result->result = res;
     return &(result->fn);
}
