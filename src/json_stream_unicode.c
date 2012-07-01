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
 * @ingroup json_utf8_stream
 * @file
 *
 * This file contains the implementation of the JSON utf8 streams.
 */

#include <stdarg.h>

#include "json_stream.h"

typedef struct json_utf8_header {
     unsigned char byte_item[4];
     int           byte_index;
     int           eof_index;
} json_utf8_header_t;

static void unicode_to_utf8(json_utf8_header_t *header, int *max_index, int unicode) {
     int mask, i;
     if (unicode < 0x80) {
          i = 0;
          header->byte_item[0] = 0;
          mask = 0x7F;
     }
     else if (unicode < 0x800) {
          i = 1;
          header->byte_item[0] = 0xC0;
          header->byte_item[1] = 0x80;
          mask = 0x1F;
     }
     else if (unicode < 0x10000) {
          i = 2;
          header->byte_item[0] = 0xE0;
          header->byte_item[1] = 0x80;
          header->byte_item[2] = 0x80;
          mask = 0x0F;
     }
     else {
          i = 3;
          header->byte_item[0] = 0xF0;
          header->byte_item[1] = 0x80;
          header->byte_item[2] = 0x80;
          header->byte_item[3] = 0x80;
          mask = 0x07;
     }
     *max_index = i;
     while (i > 0) {
          header->byte_item[i] |= (unsigned char)(unicode & 0x3F);
          unicode >>= 6;
          i--;
     }
     header->byte_item[0] |= (unsigned char)(unicode & mask);
     header->byte_index = 0;
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

typedef struct json_utf8_input_stream {
     json_input_stream_t fn;
     json_memory_t memory;
     json_input_stream_t *nested;
     json_utf8_header_t header;
} json_utf8_input_stream_t;

static void utf8_free(json_utf8_input_stream_t *this) {
     this->memory.free(this);
}

static int utf8_next(json_utf8_input_stream_t *this) {
     int result = 0;
     if (this->header.byte_index < 4) {
          this->header.byte_index++;
     }
     else {
          result = this->nested->next(this->nested);
     }
     return result;
}

static int utf8_item(json_utf8_input_stream_t *this) {
     int result;
     if (this->header.byte_index < this->header.eof_index) {
          result = this->header.byte_item[this->header.byte_index];
     }
     else {
          result = this->nested->item(this->nested);
     }
     return result;
}

static json_input_stream_t *new_utf8_stream(json_utf8_header_t header, json_input_stream_t *raw, json_memory_t memory) {
     json_utf8_input_stream_t *result = (json_utf8_input_stream_t*)memory.malloc(sizeof(json_utf8_input_stream_t));
     result->fn.free = (json_input_stream_free_fn)utf8_free;
     result->fn.next = (json_input_stream_next_fn)utf8_next;
     result->fn.item = (json_input_stream_item_fn)utf8_item;
     result->memory = memory;
     result->nested = raw;
     result->header = header;
     return &(result->fn);
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

typedef int (*read_short_fn)(json_input_stream_t *);

typedef struct json_utf16_input_stream {
     json_input_stream_t fn;
     read_short_fn read_short;
     json_memory_t memory;
     json_input_stream_t *nested;
     json_utf8_header_t header;
     int max_index;
} json_utf16_input_stream_t;

static void utf16_free(json_utf16_input_stream_t *this) {
     this->memory.free(this);
}

static int utf16_next(json_utf16_input_stream_t *this) {
     int result = 0;
     if (this->header.byte_index < this->max_index) {
          this->header.byte_index++;
     }
     else {
          int w1 = this->read_short(this->nested);
          if (w1 == -1) {
               this->header.eof_index = 0;
          }
          else if (w1 < 0xD800 || w1 > 0xDFFF) {
               unicode_to_utf8(&(this->header), &(this->max_index), w1);
          }
          else if (w1 > 0xDBFF) {
               result = -1; // error
          }
          else {
               int w2 = this->read_short(this->nested);
               if (w2 == -1) {
                    result = -1; // error
               }
               else {
                    int unicode = ((w1 & 0x3FF) << 10) | (w2 & 0x3FF) + 0x10000;
                    unicode_to_utf8(&(this->header), &(this->max_index), unicode);
               }
          }
     }
     return result;
}

static int utf16_item(json_utf16_input_stream_t *this) {
     int result = this->header.byte_index == this->header.eof_index ? -1 : this->header.byte_item[this->header.byte_index];
     return result;
}

static int utf16le_read_short(json_input_stream_t *stream) {
     int r, h, l;

     l = stream->item(stream);
     if (l == -1) {
          return -1;
     }

     r = stream->next(stream);
     if (r) {
          return -1;
     }

     h = stream->item(stream);
     if (h == -1) {
          return l;
     }

     r = stream->next(stream);
     if (r) {
          return -1;
     }

     return ((h << 8) | l) & 0xFFFF;
}

static int utf16be_read_short(json_input_stream_t *stream) {
     int r, h, l;

     h = stream->item(stream);
     if (h == -1) {
          return -1;
     }

     r = stream->next(stream);
     if (r) {
          return -1;
     }

     l = stream->item(stream);
     if (l == -1) {
          return -1;
     }

     r = stream->next(stream);
     if (r) {
          return -1;
     }

     return ((h << 8) | l) & 0xFFFF;
}

static json_utf16_input_stream_t *new_utf16_stream(json_utf8_header_t header, json_input_stream_t *raw, json_memory_t memory) {
     json_utf16_input_stream_t *result = (json_utf16_input_stream_t*)memory.malloc(sizeof(json_utf16_input_stream_t));
     result->fn.free = (json_input_stream_free_fn)utf16_free;
     result->fn.next = (json_input_stream_next_fn)utf16_next;
     result->fn.item = (json_input_stream_item_fn)utf16_item;
     result->memory = memory;
     result->nested = raw;
     result->header = header;
     return result;
}

static json_input_stream_t *new_utf16be_stream(json_utf8_header_t header, json_input_stream_t *raw, json_memory_t memory) {
     json_utf16_input_stream_t *result = new_utf16_stream(header, raw, memory);
     result->read_short = utf16be_read_short;
     result->header.byte_item[0] = result->header.byte_item[1];
     result->header.byte_item[1] = result->header.byte_item[3];
     result->max_index = 1;
     return &(result->fn);
}

static json_input_stream_t *new_utf16le_stream(json_utf8_header_t header, json_input_stream_t *raw, json_memory_t memory) {
     json_utf16_input_stream_t *result = new_utf16_stream(header, raw, memory);
     result->read_short = utf16le_read_short;
     result->header.byte_item[1] = result->header.byte_item[2];
     result->max_index = 1;
     return &(result->fn);
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

typedef int (*read_int_fn)(json_input_stream_t *);

typedef struct json_utf32_input_stream {
     json_input_stream_t fn;
     read_int_fn read_int;
     json_memory_t memory;
     json_input_stream_t *nested;
     json_utf8_header_t header;
     int max_index;
} json_utf32_input_stream_t;

static void utf32_free(json_utf32_input_stream_t *this) {
     this->memory.free(this);
}

static int utf32_next(json_utf32_input_stream_t *this) {
     int result = 0;
     if (this->header.byte_index < this->max_index) {
          this->header.byte_index++;
     }
     else {
          int i = this->read_int(this->nested);
          if (i == -1) {
               this->header.eof_index = 0;
          }
          else if (i > 0x10FFFF) {
               result = -1; // error
          }
          else {
               unicode_to_utf8(&(this->header), &(this->max_index), i);
          }
     }
     return result;
}

static int utf32_item(json_utf32_input_stream_t *this) {
     int result = this->header.byte_index == this->header.eof_index ? -1 : this->header.byte_item[this->header.byte_index];
     return result;
}

static int utf32le_read_int(json_input_stream_t *stream) {
     int r, hh, h, l, ll;

     ll = stream->item(stream);
     if (ll == -1) {
          return -1;
     }

     r = stream->next(stream);
     if (r) {
          return -1;
     }

     l = stream->item(stream);
     if (l == -1) {
          return -1;
     }

     r = stream->next(stream);
     if (r) {
          return -1;
     }

     h = stream->item(stream);
     if (h == -1) {
          return -1;
     }

     r = stream->next(stream);
     if (r) {
          return -1;
     }

     hh = stream->item(stream);
     if (hh == -1) {
          return -1;
     }

     r = stream->next(stream);
     if (r) {
          return -1;
     }

     return ((hh << 24) | (h << 16) | (l << 8) | (ll));
}

static int utf32be_read_int(json_input_stream_t *stream) {
     int r, hh, h, l, ll;

     hh = stream->item(stream);
     if (hh == -1) {
          return -1;
     }

     r = stream->next(stream);
     if (r) {
          return -1;
     }

     h = stream->item(stream);
     if (h == -1) {
          return -1;
     }

     r = stream->next(stream);
     if (r) {
          return -1;
     }

     l = stream->item(stream);
     if (l == -1) {
          return -1;
     }

     r = stream->next(stream);
     if (r) {
          return -1;
     }

     ll = stream->item(stream);
     if (ll == -1) {
          return -1;
     }

     r = stream->next(stream);
     if (r) {
          return -1;
     }
     return ((hh << 24) | (h << 16) | (l << 8) | (ll));
}

static json_utf32_input_stream_t *new_utf32_stream(json_utf8_header_t header, json_input_stream_t *raw, json_memory_t memory) {
     json_utf32_input_stream_t *result = (json_utf32_input_stream_t*)memory.malloc(sizeof(json_utf32_input_stream_t));
     result->fn.free = (json_input_stream_free_fn)utf32_free;
     result->fn.next = (json_input_stream_next_fn)utf32_next;
     result->fn.item = (json_input_stream_item_fn)utf32_item;
     result->memory = memory;
     result->nested = raw;
     result->header = header;
     return result;
}

static json_input_stream_t *new_utf32be_stream(json_utf8_header_t header, json_input_stream_t *raw, json_memory_t memory) {
     json_utf32_input_stream_t *result = new_utf32_stream(header, raw, memory);
     result->read_int = utf32be_read_int;
     result->header.byte_item[0] = result->header.byte_item[3];
     result->max_index = 0;
     return &(result->fn);
}

static json_input_stream_t *new_utf32le_stream(json_utf8_header_t header, json_input_stream_t *raw, json_memory_t memory) {
     json_utf32_input_stream_t *result = new_utf32_stream(header, raw, memory);
     result->read_int = utf32le_read_int;
     result->max_index = 0;
     return &(result->fn);
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

static json_utf8_header_t read_header(json_input_stream_t *stream) {
     json_utf8_header_t result;

     int i;
     result.eof_index = 4;
     for (i = 0; i < 4; i++) {
          int item = stream->item(stream);
          if (item == -1) {
               result.eof_index = i;
               i = 4;
          }
          else {
               result.byte_item[i] = (unsigned char)item;
               stream->next(stream);
          }
     }
     result.byte_index = 0;

     return result;
}

__PUBLIC__ json_input_stream_t *new_json_utf8_stream(json_input_stream_t *raw, json_memory_t memory) {
     json_input_stream_t *result;
     json_utf8_header_t header = read_header(raw);
     if (header.byte_item[0] == 0) {
          if (header.byte_item[1] == 0) {
               result = new_utf32be_stream(header, raw, memory);
          }
          else {
               result = new_utf16be_stream(header, raw, memory);
          }
     }
     else if (header.byte_item[1] == 0) {
          if (header.byte_item[2] == 0) {
               result = new_utf32le_stream(header, raw, memory);
          }
          else {
               result = new_utf16le_stream(header, raw, memory);
          }
     }
     else {
          result = new_utf8_stream(header, raw, memory);
     }
     return result;
}
