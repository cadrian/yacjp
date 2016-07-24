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

static void on_error(cad_input_stream_t *s, int line, int column, void *data, const char *format, ...) {
     assert(0);
}

static char *source = "{\"true\":true, \"false\": false, \"null\": null}";

int main() {
     json_value_t *value;
     json_object_t *root;
     json_const_t *c_true, *c_false, *c_null;
     stream = new_cad_input_stream_from_string(source, stdlib_memory);
     value = json_parse(stream, on_error, NULL, stdlib_memory);

     root = (json_object_t*)value;
     c_true = (json_const_t*)(root->get(root, "true"));
     c_false = (json_const_t*)(root->get(root, "false"));
     c_null = (json_const_t*)(root->get(root, "null"));

     assert(c_true->value(c_true) == json_true);
     assert(c_false->value(c_false) == json_false);
     assert(c_null->value(c_null) == json_null);

     return 0;
}
