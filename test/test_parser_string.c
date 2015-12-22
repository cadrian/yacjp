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

#include "test.h"
#include "json.h"

static cad_input_stream_t *stream;

static void on_error(cad_input_stream_t *s, int line, int column, const char *format, ...) {
     assert(0);
}

static char *source = "{\"key\":[1, 2], \"foo\": \"data\", \"bar\": {\"a\": 1.4e9}, \"zero_dot_four\": 0.4, \"negative_zero_dot_four\": -0.4}";

int main() {
     json_value_t *value;
     json_object_t *root;
     json_number_t *zero_dot_four, *negative_zero_dot_four;
     stream = new_cad_input_stream_from_string(source, stdlib_memory);
     value = json_parse(stream, on_error, stdlib_memory);

     root = (json_object_t*)value;
     zero_dot_four = (json_number_t*)(root->get(root, "zero_dot_four"));
     negative_zero_dot_four = (json_number_t*)(root->get(root, "negative_zero_dot_four"));

     assert(!zero_dot_four->is_int(zero_dot_four));
     assert(abs(zero_dot_four->to_double(zero_dot_four) - (double)0.4) < 1e-9); // 0.4 cannot be reliably coded on a binary system, expect a small deviance

     assert(!negative_zero_dot_four->is_int(negative_zero_dot_four));
     assert(negative_zero_dot_four->to_double(negative_zero_dot_four) < 0);
     assert(abs(negative_zero_dot_four->to_double(negative_zero_dot_four) - (double)-0.4) < 1e-9); // -0.4 cannot be reliably coded on a binary system, expect a small deviance

     return 0;
}
