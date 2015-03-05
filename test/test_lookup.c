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

#include <stdarg.h>
#include <string.h>

#include "test.h"
#include "json.h"

static json_input_stream_t *stream;

static void on_error(json_input_stream_t *s, int line, int column, const char *format, ...) {
     char *a = 0;
     va_list args;
     assert(stream == s);
     va_start(args, format);
     vfprintf(stderr, format, args);
     va_end(args);
     *a=0;
}

json_value_t *json_vlookup_wrapper(json_value_t *value, ...) {
    va_list args;
    va_start(args, value);
    return json_vlookup(value, args);
    va_end(args);
}

int main() {
     json_value_t *value;
     FILE *file = fopen("target/out/data/config.ini", "r");
     assert(file != NULL);

     stream = new_json_input_stream_from_file(file, stdlib_memory);
     value = json_parse(stream, on_error, stdlib_memory);
     fclose(file);

     {
          json_object_t *object = (json_object_t*)value;
          json_object_t *mainobj;
          json_number_t *fullscreen;
          json_number_t *width;
          json_number_t *height;
          json_string_t *profile;
          char profile_value[8];
          int count;

          mainobj = (json_object_t*)json_lookup(value, "main", JSON_STOP);
          assert(mainobj == (json_object_t*)object->get(object, "main"));

          fullscreen = (json_number_t*)json_lookup(value, "main", "fullscreen");
          assert(fullscreen == (json_number_t*)mainobj->get(mainobj, "fullscreen"));
          assert(fullscreen->is_int(fullscreen));
          assert(fullscreen->to_int(fullscreen) == 0);
          assert(fullscreen->to_double(fullscreen) == 0.0);

          width = (json_number_t*)json_lookup(value, "main", "width");
          assert(width == (json_number_t*)mainobj->get(mainobj, "width"));
          assert(width->is_int(width));
          assert(width->to_int(width) == 800);
          assert(width->to_double(width) == 800.0);

          height = (json_number_t*)json_lookup(value, "main", "height");
          assert(height == (json_number_t*)mainobj->get(mainobj, "height"));
          assert(height->is_int(height));
          assert(height->to_int(height) == 480);
          assert(height->to_double(height) == 480.0);

          profile = (json_string_t*)json_lookup(value, "main", "profile");
          assert(profile == (json_string_t*)mainobj->get(mainobj, "profile"));
          memset(profile_value, 1, 8); // not \0 to ensure that the NUL character is correctly written by the utf8 converter
          count = profile->utf8(profile, profile_value, 8);
          assert(count == 4);
          assert(0 == strcmp("test", profile_value));

          // variadic version
          mainobj = (json_object_t*)json_vlookup_wrapper(value, "main", JSON_STOP);
          assert(mainobj == (json_object_t*)object->get(object, "main"));

          fullscreen = (json_number_t*)json_vlookup_wrapper(value, "main", "fullscreen");
          assert(fullscreen == (json_number_t*)mainobj->get(mainobj, "fullscreen"));
          assert(fullscreen->is_int(fullscreen));
          assert(fullscreen->to_int(fullscreen) == 0);
          assert(fullscreen->to_double(fullscreen) == 0.0);

          width = (json_number_t*)json_vlookup_wrapper(value, "main", "width");
          assert(width == (json_number_t*)mainobj->get(mainobj, "width"));
          assert(width->is_int(width));
          assert(width->to_int(width) == 800);
          assert(width->to_double(width) == 800.0);

          height = (json_number_t*)json_vlookup_wrapper(value, "main", "height");
          assert(height == (json_number_t*)mainobj->get(mainobj, "height"));
          assert(height->is_int(height));
          assert(height->to_int(height) == 480);
          assert(height->to_double(height) == 480.0);

          profile = (json_string_t*)json_vlookup_wrapper(value, "main", "profile");
          assert(profile == (json_string_t*)mainobj->get(mainobj, "profile"));
          memset(profile_value, 1, 8); // not \0 to ensure that the NUL character is correctly written by the utf8 converter
          count = profile->utf8(profile, profile_value, 8);
          assert(count == 4);
          assert(0 == strcmp("test", profile_value));
     }

     return 0;
}
