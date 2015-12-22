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

#include <stdio.h>

#include "test.h"
#include "json.h"

static void fill_object(json_object_t *object, unsigned int depth) {
     static unsigned long count = 0;

     assert(object != NULL);

     json_object_t *foo = json_new_object(stdlib_memory);
     json_object_t *bar = json_new_object(stdlib_memory);
     json_string_t *string = json_new_string(stdlib_memory);
     json_number_t *number = json_new_number(stdlib_memory);

     object->set(object, "foo",    (json_value_t*)foo);
     object->set(object, "bar",    (json_value_t*)bar);
     object->set(object, "string", (json_value_t*)string);
     object->set(object, "number", (json_value_t*)number);

     count++;
     string->add_string(string, "string #%lu at depth %u", count, depth);
     number->set(number, 1, (int)count, 0, 0, 0);

     if (depth > 0) {
          json_object_t *more = json_new_object(stdlib_memory);
          fill_object(more, depth - 1);
          object->set(object, "more", (json_value_t*)more);
          fill_object(foo, depth / 100);
     }
}

int main() {
     json_object_t *object = json_new_object(stdlib_memory);
     cad_output_stream_t *out = new_cad_output_stream_from_file(stdout, stdlib_memory);
     fill_object(object, 1000);

     object->accept(object, json_write_to(out, stdlib_memory, json_extend_spaces));
     object->accept(object, json_kill());

     return 0;
}
