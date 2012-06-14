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

#define BUFFER_SIZE 4096

struct json_stream_file_descriptor {
   struct json_stream fn;
   int fd;
   char buffer[BUFFER_SIZE];
   int max;
   int index;
};

static int next(struct json_stream_file_descriptor *this) {
   int result = 0;
   if (this->max) {
      if (this->index < this->max) {
         this->index++;
      }
      else {
         this->max = read(this->fd, this->buffer, BUFFER_SIZE);
         this->index = 0;
         if (this->max < 0) {
            result = -1;
         }
      }
   }
   return result;
}

static int item(struct json_stream_file_descriptor *this) {
   if (this->max == 0) {
      return EOF;
   }
   return this->buffer[this->index];
}

__PUBLIC__ json_stream_t *new_json_stream_from_file_description(int fd) {
   struct json_stream_file_descriptor *result = (struct json_stream_file_descriptor *)malloc(sizeof(struct json_stream_file_descriptor));
   if (!result) return NULL;
   result->fn.next = (next_fn)next;
   result->fn.item = (item_fn)item;
   result->fd = fd;
   result->max = read(fd, result->buffer, BUFFER_SIZE);
   result->index = 0;
   return &(result->fn);
}
