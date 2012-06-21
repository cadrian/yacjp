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

#include <alloca.h>
#include <string.h>

#include "json_value.h"
#include "json_stream.h"

__PUBLIC__ short json_compact        = 0x00;
__PUBLIC__ short json_extend_unicode = 0x01;
__PUBLIC__ short json_extend_spaces  = 0x02;

typedef struct json_writer {
     json_visitor_t fn;
     json_memory_t memory;

     json_output_stream_t *stream;
     short options;
     int depth;
     char *buffer;
     int   capacity;
} json_writer_t;

static void del(json_writer_t *this) {
     this->memory.free(this->buffer);
     this->memory.free(this);
}

static void newline_and_indent(json_writer_t *this) {
     if (this->options & json_extend_spaces) {
          this->stream->put(this->stream, "\n%*c", this->depth * 4, ' ');
     }
}

static void write_object(json_writer_t *this, json_object_t *visited) {
     int i, key_space = 0;
     int n = visited->count(visited);
     const char **keys = (const char **)alloca(n * sizeof(const char*));
     visited->keys(visited, keys);
     if (this->options & json_extend_spaces) {
          for (i = 0; i < n; i++) {
               int ks = strlen(keys[i]);
               if (ks > key_space) key_space = ks;
          }
     }
     this->stream->put(this->stream, "{");
     this->depth++;
     newline_and_indent(this);
     for (i = 0; i < n; i++) {
          json_value_t *v = visited->get(visited, keys[i]);
          if (i > 0) {
               this->stream->put(this->stream, ",");
          }
          newline_and_indent(this);
          this->stream->put(this->stream, "\"%s\":", keys[i]);
          if (this->options & json_extend_spaces) {
               this->stream->put(this->stream, "%*c", key_space, ' ');
          }
          v->accept(v, (json_visitor_t*)this);
     }
     this->depth--;
     newline_and_indent(this);
     this->stream->put(this->stream, "}");
}

static void write_array(json_writer_t *this, json_array_t  *visited) {
     int n = visited->count(visited);
     int i;
     this->stream->put(this->stream, "[");
     this->depth++;
     newline_and_indent(this);
     for (i = 0; i < n; i++) {
          json_value_t *v = visited->get(visited, i);
          if (i > 0) {
               this->stream->put(this->stream, ",");
          }
          newline_and_indent(this);
          v->accept(v, (json_visitor_t*)this);
     }
     visited->free(visited);
     this->depth--;
     newline_and_indent(this);
     this->stream->put(this->stream, "]");
}

static void write_character(json_writer_t *this, int unicode) {
     switch(unicode) {
     case '"': case '\\':
          this->stream->put(this->stream, "\\%c", (char)unicode);
          break;
     case '\b':
          this->stream->put(this->stream, "\\b");
          break;
     case '\f':
          this->stream->put(this->stream, "\\f");
          break;
     case '\n':
          this->stream->put(this->stream, "\\n");
          break;
     case '\r':
          this->stream->put(this->stream, "\\r");
          break;
     case '\t':
          this->stream->put(this->stream, "\\t");
          break;
     default:
          if (unicode >= 32 && unicode < 127) {
               this->stream->put(this->stream, "%c", (char)unicode);
          }
          else if (this->options & json_extend_unicode) {
               this->stream->put(this->stream, "\\u%04x", unicode);
          }
          else {
               this->stream->put(this->stream, "%c", (char)unicode);
          }
     }
}

static void write_string(json_writer_t *this, json_string_t *visited) {
     this->stream->put(this->stream, "\"");
     if (this->options & json_extend_unicode) {
          int i, n = visited->count(visited);
          for (i = 0; i < n; i++) {
               write_character(this, visited->get(visited, i));
          }
     }
     else {
          char *buffer;
          int new_capacity = this->capacity;
          int n = visited->utf8(visited, this->buffer, new_capacity);
          if (new_capacity <= n) {
               do {
                    new_capacity <<= 1;
               } while (new_capacity <= n);

               this->memory.free(this->buffer);
               this->buffer = this->memory.malloc(new_capacity);
               visited->utf8(visited, this->buffer, new_capacity);
          }
          buffer = this->buffer;
          while (*buffer) {
               write_character(this, *buffer);
               buffer++;
          }
     }
     this->stream->put(this->stream, "\"");
}

static void write_number(json_writer_t *this, json_number_t *visited) {
     int new_capacity = this->capacity;
     int n = visited->to_string(visited, this->buffer, new_capacity);
     if (new_capacity <= n) {
          do {
               new_capacity <<= 1;
          } while (new_capacity <= n);

          this->memory.free(this->buffer);
          this->buffer = this->memory.malloc(new_capacity);
          this->capacity = new_capacity;
          visited->to_string(visited, this->buffer, new_capacity);
     }
     this->stream->put(this->stream, "%s", this->buffer);
}

static void write_const(json_writer_t *this, json_const_t  *visited) {
     switch(visited->value(visited)) {
     case json_false:
          this->stream->put(this->stream, "false");
          break;
     case json_true:
          this->stream->put(this->stream, "true");
          break;
     case json_null:
          this->stream->put(this->stream, "null");
          break;
     }
}

__PUBLIC__ json_visitor_t *json_write_to(json_output_stream_t *stream, json_memory_t memory, short options) {
     json_writer_t *result = (json_writer_t*)memory.malloc(sizeof(json_writer_t));
     result->fn.visit_object = (json_visit_object_fn)write_object;
     result->fn.visit_array  = (json_visit_array_fn )write_array ;
     result->fn.visit_string = (json_visit_string_fn)write_string;
     result->fn.visit_number = (json_visit_number_fn)write_number;
     result->fn.visit_const  = (json_visit_const_fn )write_const ;
     result->memory  = memory;
     result->stream  = stream;
     result->options = options;
     result->depth   = 0;
     result->buffer   = (char*)memory.malloc(1024);
     result->capacity = 1024;
     return &(result->fn);
}
