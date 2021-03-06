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

static int error_data = 42;

static void on_error(cad_input_stream_t *s, int line, int column, void *data, const char *format, ...) {
     assert(line==4);
     assert(column==20);
     assert(data == &error_data);
     assert(*((int*)data) == 42);
}

static char *source = "{\n\"key\":[1, 2],\n\"foo\": \"data\",\n\"bar\": {\"a\": 1.4e9}:\n}";

int main() {
     json_value_t *value;
     stream = new_cad_input_stream_from_string(source, stdlib_memory);
     value = json_parse(stream, on_error, &error_data, stdlib_memory);

     assert(value != NULL);

     return 0;
}
