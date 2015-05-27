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
 * @ingroup json_stream
 * @file
 *
 * This file contains the implementation of the JSON string streams.
 */

#include <string.h>
#include <stdarg.h>

#include "json_stream.h"

struct json_input_stream_string {
     struct json_input_stream fn;
     cad_memory_t memory;

     const char *string;
     int index;
};

static void free_input(struct json_input_stream_string *this) {
     this->memory.free(this);
}

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

static json_input_stream_t input_fn = {
     (json_input_stream_free_fn)free_input,
     (json_input_stream_next_fn)next      ,
     (json_input_stream_item_fn)item      ,
};

__PUBLIC__ json_input_stream_t *new_json_input_stream_from_string(const char *string, cad_memory_t memory) {
     struct json_input_stream_string *result = (struct json_input_stream_string *)memory.malloc(sizeof(struct json_input_stream_string));
     if (!result) return NULL;
     result->fn     = input_fn;
     result->memory = memory;
     result->string = string;
     result->index  = 0;
     return &(result->fn);
}



struct json_output_stream_string {
     struct json_output_stream fn;
     cad_memory_t memory;

     char **string;
     int capacity;
     int count;
};

static void free_output(struct json_input_stream_string *this) {
     this->memory.free(this);
}

static void put(struct json_output_stream_string *this, const char *format, ...) {
     int c;
     int new_capacity = this->capacity;
     char *string = *(this->string);
     char *new_string = string;
     va_list args;

     va_start(args, format);
     c = vsnprintf("", 0, format, args) + 1;
     va_end(args);

     if (new_capacity == 0) {
          new_capacity = 128;
     }
     while (c + this->count > new_capacity) {
          new_capacity *= 2;
     }
     if (new_capacity > this->capacity) {
          new_string = (char *)this->memory.malloc(new_capacity);
          if (string) {
               strcpy(new_string, string);
          }
          *(this->string) = new_string;
          this->capacity = new_capacity;
     }

     va_start(args, format);
     this->count += vsprintf(new_string + this->count, format, args);
     va_end(args);
}

static void flush(struct json_output_stream_string *this) {
     /* do nothing */
}

static json_output_stream_t output_fn = {
     (json_output_stream_free_fn )free_output,
     (json_output_stream_put_fn  )put        ,
     (json_output_stream_flush_fn)flush      ,
};

__PUBLIC__ json_output_stream_t *new_json_output_stream_from_string(char **string, cad_memory_t memory) {
     struct json_output_stream_string *result = (struct json_output_stream_string *)memory.malloc(sizeof(struct json_output_stream_string));
     if (!result) return NULL;
     result->fn       = output_fn;
     result->memory   = memory;
     result->string   = string;
     result->capacity = 0;
     result->count    = 0;
     *string = NULL;
     return &(result->fn);
}
