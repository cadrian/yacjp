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
 * This file contains the implementation of the JSON file streams.
 */

#include <stdarg.h>

#include "json_stream.h"

#define BUFFER_SIZE 4096

struct json_input_stream_file {
     struct json_input_stream fn;
     json_memory_t memory;

     FILE *file;
     char buffer[BUFFER_SIZE];
     int max;
     int index;
};

static void free_input(struct json_input_stream_file *this) {
     this->memory.free(this);
}

static int next(struct json_input_stream_file *this) {
     int result = 0;
     if (this->max) {
          this->index++;
          if (this->index >= this->max) {
               this->max = fread(this->buffer, sizeof(char), BUFFER_SIZE, this->file);
               this->index = 0;
               if (this->max == 0 && ferror(this->file)) {
                    result = -1;
               }
          }
     }
     return result;
}

static int item(struct json_input_stream_file *this) {
     if (this->max == 0) {
          return EOF;
     }
     return this->buffer[this->index];
}

__PUBLIC__ json_input_stream_t *new_json_input_stream_from_file(FILE *file, json_memory_t memory) {
     struct json_input_stream_file *result = (struct json_input_stream_file *)memory.malloc(sizeof(struct json_input_stream_file));
     if (!result) return NULL;
     result->fn.free = (json_input_stream_free_fn)free_input;
     result->fn.next = (json_input_stream_next_fn)next;
     result->fn.item = (json_input_stream_item_fn)item;
     result->memory  = memory;
     result->file    = file;
     result->max     = -1;
     result->index   = 0;
     next(result);
     return &(result->fn);
}



struct json_output_stream_file {
     struct json_output_stream fn;
     json_memory_t memory;

     FILE *file;
};

static void free_output(struct json_input_stream_file *this) {
     this->memory.free(this);
}

static void put(struct json_output_stream_file *this, const char *format, ...) {
     va_list args;
     va_start(args, format);
     vfprintf(this->file, format, args);
     va_end(args);
}

static void flush(struct json_output_stream_file *this) {
     fflush(this->file);
}

__PUBLIC__ json_output_stream_t *new_json_output_stream_from_file(FILE *file, json_memory_t memory) {
     struct json_output_stream_file *result = (struct json_output_stream_file *)memory.malloc(sizeof(struct json_output_stream_file));
     if (!result) return NULL;
     result->fn.free  = (json_output_stream_free_fn )free_output;
     result->fn.put   = (json_output_stream_put_fn  )put;
     result->fn.flush = (json_output_stream_flush_fn)flush;
     result->memory   = memory;
     result->file     = file;
     return &(result->fn);
}
