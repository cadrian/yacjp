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

static cad_input_stream_t *stream;
static cad_output_stream_t *out;

static void on_error(cad_input_stream_t *s, int line, int column, const char *format, ...) {
     assert(0);
}

int main() {
     json_value_t *value;
     json_visitor_t *writer;
     char *out_source;
     char expected[1024];
     FILE *file = fopen("target/out/data/config.ini", "r");
     int n;
     assert(file != NULL);

     stream = new_cad_input_stream_from_file(file, stdlib_memory);
     value = json_parse(stream, on_error, stdlib_memory);
     fclose(file);

     out = new_cad_output_stream_from_string(&out_source, stdlib_memory);
     assert(NULL == out_source);
     writer = json_write_to(out, stdlib_memory, json_extend_spaces);
     value->accept(value, writer);
     out->put(out, "\n"); // last \n not handled by the visitor
     assert(NULL != out_source);

     memset(expected, 0, 1024);
     file = fopen("target/out/data/config.ref", "r");
     n = fread(expected, 1, 1024, file);
     fclose(file);

     assert(n == strlen(expected));
     assert(0 == strcmp(expected, out_source));

     return 0;
}
