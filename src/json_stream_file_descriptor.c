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
 * This file contains the implementation of the JSON file descriptor streams.
 */

#include "json_stream.h"

#define BUFFER_SIZE 4096

struct json_input_stream_file_descriptor {
     struct json_input_stream fn;
     json_memory_t memory;

     int fd;
     char buffer[BUFFER_SIZE];
     int max;
     int index;
};

static int next(struct json_input_stream_file_descriptor *this) {
     int result = 0;
     if (this->max) {
          this->index++;
          if (this->index >= this->max) {
               this->max = read(this->fd, this->buffer, BUFFER_SIZE);
               this->index = 0;
               if (this->max < 0) {
                    result = -1;
               }
          }
     }
     return result;
}

static int item(struct json_input_stream_file_descriptor *this) {
     if (this->max == 0) {
          return EOF;
     }
     return this->buffer[this->index];
}

__PUBLIC__ json_input_stream_t *new_json_input_stream_from_file_descriptor(int fd, json_memory_t memory) {
     struct json_input_stream_file_descriptor *result = (struct json_input_stream_file_descriptor *)memory.malloc(sizeof(struct json_input_stream_file_descriptor));
     if (!result) return NULL;
     result->fn.next = (json_input_stream_next_fn)next;
     result->fn.item = (json_input_stream_item_fn)item;
     result->memory  = memory;
     result->fd      = fd;
     result->max     = -1;
     result->index   = 0;
     next(result);
     return &(result->fn);
}



struct json_output_stream_file {
     struct json_output_stream fn;
     json_memory_t memory;

     int fd;
};

__PUBLIC__ json_output_stream_t *new_json_output_stream_from_file_descriptor(int fd, json_memory_t memory) {
     /* not (yet) supported */
     return NULL;
}
