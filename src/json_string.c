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
 * @ingroup json_string
 * @file
 *
 * This file contains the implementation of the JSON strings.
 */

#include <stdarg.h>
#include <alloca.h>
#include <string.h>

#include "json_value.h"

typedef struct low_surrogate {
     int        index;
     __uint16_t value;
} low_surrogate_t;

struct json_string_impl {
     struct json_string fn;
     json_memory_t memory;

     int              string_count;
     int              string_capacity;
     __uint16_t      *string;

     int              low_surrogates_count;
     int              low_surrogates_capacity;
     low_surrogate_t *low_surrogates;

     int              accu_expected;
     int              accu_count;
     unicode_char_t   accu;
};

static void grow_low_surrogates(struct json_string_impl *this) {
     int new_capacity = this->low_surrogates_capacity;
     low_surrogate_t *new_low_surrogates;
     if (new_capacity == 0) {
          new_capacity = 4;
     }
     else {
          new_capacity <<= 1;
     }
     new_low_surrogates = (low_surrogate_t *)this->memory.malloc(new_capacity * sizeof(low_surrogate_t));
     if (this->low_surrogates) {
          memcpy(new_low_surrogates, this->low_surrogates, this->low_surrogates_capacity * sizeof(low_surrogate_t));
          this->memory.free(this->low_surrogates);
     }
     this->low_surrogates_capacity = new_capacity;
     this->low_surrogates = new_low_surrogates;
}

static void grow_string(struct json_string_impl *this) {
     int new_capacity = this->string_capacity << 1;
     __uint16_t *new_string = (__uint16_t *)this->memory.malloc(new_capacity * sizeof(__uint16_t));
     memcpy(new_string, this->string, this->string_capacity * sizeof(__uint16_t));
     this->memory.free(this->string);
     this->string_capacity = new_capacity;
     this->string = new_string;
}

static int get_low_surrogate_position(struct json_string_impl *this, int index) {
     int low = 0, high = this->low_surrogates_count, medium;

     if (index == this->low_surrogates[low].index) {
          return low;
     }
     if (index == this->low_surrogates[high].index) {
          return high;
     }

     do {
          medium = (high+1)>>1;
          if (index == this->low_surrogates[medium].index) {
               return medium;
          }
          else if (index > this->low_surrogates[medium].index) {
               low = medium + 1;
          }
          else {
               high = medium - 1;
          }
     } while (high > low);

     return -medium - 1;
}

static __uint16_t get_low_surrogate(struct json_string_impl *this, int index) {
     /* should be called only if we are sure the index is in the low surrogates table */
     return this->low_surrogates[get_low_surrogate_position(this, index)].value;
}

static void set_low_surrogate(struct json_string_impl *this, int index, __uint16_t value) {
     low_surrogate_t low_surrogate = {index, value};

     int pos = get_low_surrogate_position(this, index);
     if (pos < 0) {
          if (this->low_surrogates_capacity == this->low_surrogates_count) {
               grow_low_surrogates(this);
          }
          pos = -pos - 1;
          memmove(this->low_surrogates + pos + 1, this->low_surrogates + pos, this->low_surrogates_count - pos);
          this->low_surrogates_count++;
     }

     this->low_surrogates[pos] = low_surrogate;
}



static void accept(struct json_string_impl *this, json_visitor_t *visitor) {
     visitor->visit_string(visitor, (json_string_t*)this);
}

static unicode_char_t get(struct json_string_impl *this, unsigned int index) {
     unicode_char_t result = this->string[index];
     if (result & 0x0000F800 == 0x0000D800) {
          result = result & 0x000007FF + 64;
          result = result * 1024 + get_low_surrogate(this, index);
     }
     else {
          result = result & 0x0000FFFF;
     }
}

static int valid_unicode(unicode_char_t v) {
     return v >= 0 && v <= 0x0010FFFF
          && !(v >= 0x0000D800 && v <= 0x0000DFFF)
          && v != 0x0000FFFE
          && v != 0x0000FFFF;
}

static void add_unicode(struct json_string_impl *this, int unicode) {
     if (this->string_count == this->string_capacity) {
          grow_string(this);
     }
     if (unicode >= 65536) {
          set_low_surrogate(this, this->string_count, (__uint16_t)(unicode & 0x000003FF));
          this->string[this->string_count] = (__uint16_t)((unicode / 1024 - 64) & 0x0000FFFF);
     }
     else {
          this->string[this->string_count] = (__uint16_t)unicode;
     }
     this->string_count++;
}

static int add(struct json_string_impl *this, char c) {
     int result;
     int k;
     unicode_char_t v = (unicode_char_t)c;

     if (this->accu_count == 0) {
          if (v < 128) {
               add_unicode(this, v);
               k = 0;
          }
          else if (v >= 192 && v < 224) {
               v -= 192;
               k = 2;
          }
          else if (v >= 224 && v < 240) {
               v -= 224;
               k = 3;
          }
          else if (v >= 240 && v < 248) {
               v -= 240;
               k = 4;
          }
          else {
               add_unicode(this, (unicode_char_t)65533);
               k = -1;
          }
          if (k == -1) {
               this->accu_expected = this->accu_count = 0;
               result = -1;
          }
          else {
               result = this->accu_expected = this->accu_count = k;
          }
          this->accu = v;
     }
     else {
          if (this->accu_count > 1) {
               if (v >= 128 && v < 192) {
                    this->accu = this->accu * 64 + v - 128;
                    result = --this->accu_count;
               }
               else {
                    add_unicode(this, (unicode_char_t)65533);
                    this->accu_count = 0;
                    result = -1;
               }
          }
          if (this->accu_count == 1) {
               if ((this->accu < 128)
                   || (this->accu < 2048 && this->accu_expected > 2)
                   || (this->accu < 65536 && this->accu_expected > 3)
                   || !valid_unicode(this->accu)) {
                    add_unicode(this, (unicode_char_t)65533);
                    result = -1;
               }
               else {
                    add_unicode(this, this->accu);
                    result = 0;
               }
               this->accu_count = 0;
          }
     }

     return result;
}

#define DEFAULT_BUFFER_SIZE 128

static void add_string(struct json_string_impl *this, char *format, ...) {
     char data0[DEFAULT_BUFFER_SIZE];
     char *data = data0;
     int n, ok;
     unicode_char_t v;
     va_list args;

     va_start(args, format);
     n = vsnprintf(data, DEFAULT_BUFFER_SIZE, format, args);
     va_end(args);

     if (n > DEFAULT_BUFFER_SIZE) {
          data = (char*)alloca(n+1);

          va_start(args, format);
          vsnprintf(data, n+1, format, args);
          va_end(args);
     }

     while (*data) {
          v = *data;
          data++;
          n = add(this, v);
          ok = n > 0;
          while (n --> 0) {
               ok &= add(this, *data) > 0;
               data++;
          }
     }
}

static int count(struct json_string_impl *this) {
     return this->string_count;
}

#define add_to_buffer(v) do {if (result < size) buffer[result] = (char)((v)&0x7f); result++;} while(0)

static size_t utf8(struct json_string_impl *this, char *buffer, size_t size) {
     size_t result = 0;
     int i;
     low_surrogate_t ls;
     for (i = 0; i < this->string_count; i++) {
          int v = this->string[i];
          if (v < 128) {
               add_to_buffer(v);
          }
          else {
               if (v < 2048) {
                    add_to_buffer(v / 64 + 192);
               }
               else {
                    if (v < 65536) {
                         add_to_buffer(v / 0x00001000 + 224);
                    }
                    else {
                         add_to_buffer(v / 0x00040000 + 240);
                         v %= 0x00040000;
                         add_to_buffer(v / 0x00001000 + 128);
                    }
                    v %= 0x00001000;
                    add_to_buffer(v / 64 + 128);
               }
               add_to_buffer(v % 64 + 128);
          }
     }
     add_to_buffer(0); result--; // don't count \0
     return result;
}

static void free_(struct json_string_impl *this) {
     if (this->string) this->memory.free(this->string);
     this->memory.free(this);
}

static json_string_t fn = {
     (json_string_accept_fn    )accept     ,
     (json_string_free_fn      )free_      ,
     (json_string_count_fn     )count      ,
     (json_string_utf8_fn      )utf8       ,
     (json_string_get_fn       )get        ,
     (json_string_add_string_fn)add_string ,
     (json_string_add_utf8_fn  )add        ,
     (json_string_add_fn       )add_unicode,
};

__PUBLIC__ json_string_t *json_new_string(json_memory_t memory) {
     struct json_string_impl *result = (struct json_string_impl *)memory.malloc(sizeof(struct json_string_impl));
     if (!result) return NULL;
     result->fn              = fn;
     result->memory          = memory;

     result->string_capacity = 4;
     result->string_count    = 0;
     result->string          = (__uint16_t*)memory.malloc(4 * sizeof(__uint16_t));

     result->low_surrogates_capacity = 0;
     result->low_surrogates_count    = 0;
     result->low_surrogates          = NULL;

     result->accu_expected = 0;
     result->accu_count    = 0;
     result->accu          = 0;

     return &(result->fn);
}
