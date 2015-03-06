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

#include <string.h>

#include "test.h"
#include "json.h"

static json_input_stream_t *stream;
static json_output_stream_t *out;

static void on_error(json_input_stream_t *s, int line, int column, const char *format, ...) {
     assert(0);
}

static char *source = "{\"foo\":\"data\",\"key\":[1,2],\"bat\":{\"a\":1.4e+9}}";

int main() {
     json_value_t *value = NULL, *clone = NULL;
     json_visitor_t *writer = NULL, *cloner = NULL;
     char *out_source = NULL;

     stream = new_json_input_stream_from_string(source, stdlib_memory);
     value = json_parse(stream, on_error, stdlib_memory);

     assert(NULL != value);

     out = new_json_output_stream_from_string(&out_source, stdlib_memory);
     assert(NULL == out_source);

     cloner = json_clone(&clone, stdlib_memory);
     value->accept(value, cloner);

     assert(NULL != clone);

     writer = json_write_to(out, stdlib_memory, json_compact);
     clone->accept(clone, writer);

     assert(NULL != out_source);
     assert(0 == strcmp(source, out_source));

     return 0;
}
