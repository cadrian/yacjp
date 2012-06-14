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

struct json_stream_file {
   struct json_stream fn;
   FILE *file;
   char buffer[BUFFER_SIZE];
   int max;
   int index;
};

static int next(struct json_stream_file *this) {
   int result = 0;
   if (this->max) {
      if (this->index < this->max) {
         this->index++;
      }
      else {
         this->max = fread(this->buffer, sizeof(char), BUFFER_SIZE, this->file);
         this->index = 0;
         if (this->max == 0 && ferror(this->file)) {
            result = -1;
         }
      }
   }
   return result;
}

static int item(struct json_stream_file *this) {
   if (this->max == 0) {
      return EOF;
   }
   return this->buffer[this->index];
}

__PUBLIC__ json_stream_t *new_json_stream_from_file(FILE *file) {
   struct json_stream_file *result = (struct json_stream_file *)malloc(sizeof(struct json_stream_file));
   if (!result) return NULL;
   result->fn.next = (next_fn)next;
   result->fn.item = (item_fn)item;
   result->file = file;
   result->max = fread(result->buffer, sizeof(char), BUFFER_SIZE, file);
   result->index = 0;
   return &(result->fn);
}
