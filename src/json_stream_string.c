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

#include "json_stream.h"

struct json_input_stream_string {
     struct json_input_stream fn;
     json_memory_t memory;

     char *string;
     int index;
};

static int next(struct json_input_stream_string *this) {
     if (this->string[this->index]) {
          this->index++;
     }
     return 0;
}

static int item(struct json_input_stream_string *this) {
     int result = this->string[this->index];
     return result ? result : EOF;
}

__PUBLIC__ json_input_stream_t *new_json_input_stream_from_string(char *string, json_memory_t memory) {
     struct json_input_stream_string *result = (struct json_input_stream_string *)memory.malloc(sizeof(struct json_input_stream_string));
     if (!result) return NULL;
     result->fn.next = (next_fn)next;
     result->fn.item = (item_fn)item;
     result->memory  = memory;
     result->string  = string;
     result->index   = 0;
     return &(result->fn);
}
