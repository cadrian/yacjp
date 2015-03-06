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

static void visit_value(struct json_visitor_clone *this, json_value_t *visited) {
     char *buffer;
     json_output_stream_t *out;
     json_input_stream_t *in;

     out = new_json_output_stream_from_string(&buffer, this->memory);
     json_write_to(out, this->memory, 0);

     in = new_json_input_stream_from_string(buffer, this->memory);
     *(this->result) = json_parse(in, NULL, this->memory);

     this->memory.free(buffer);
     this->memory.free(out);
}

static void free_(struct json_visitor_clone *this) {
     this->memory.free(this);
}

static json_visitor_t fn = {
     (json_visit_free_fn  ) free_,
     (json_visit_object_fn) visit_value,
     (json_visit_array_fn ) visit_value,
     (json_visit_string_fn) visit_value,
     (json_visit_number_fn) visit_value,
     (json_visit_const_fn ) visit_value,
};

__PUBLIC__ json_visitor_t *json_clone(json_value_t **res, cad_memory_t memory) {
     struct json_visitor_clone *result = malloc(sizeof(struct json_visitor_clone));
     result->fn = fn;
     result->memory = memory;
     result->result = res;
     return &(result->fn);
}
