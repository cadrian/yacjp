/*
  This file is part of YacJP.

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

#include "json.h"

static void default_on_error(json_input_stream_t *stream, int line, int column, const char *format, ...) {
     va_list args;
     va_start(args, format);
     fprintf(stderr, "**** Syntax error line %d, column %d: ", line, column);
     vfprintf(stderr, format, args);
     fprintf(stderr, "\n");
     va_end(args);
}

struct json_parse_context {
     json_input_stream_t *stream;
     json_on_error_fn on_error;
     int line;
     int column;
     json_memory_t memory;
     char *utf8_buffer;
     int   utf8_capacity;
};

static json_value_t  *parse_value (struct json_parse_context *context);
static json_object_t *parse_object(struct json_parse_context *context);
static json_array_t  *parse_array (struct json_parse_context *context);
static json_number_t *parse_number(struct json_parse_context *context);
static json_string_t *parse_string(struct json_parse_context *context);
static json_const_t  *parse_true  (struct json_parse_context *context);
static json_const_t  *parse_false (struct json_parse_context *context);
static json_const_t  *parse_null  (struct json_parse_context *context);

#define error(context, message, ...) (context)->on_error((context)->stream, (context)->line, (context)->column, message, __VA_ARGS__)
#define item(context) ((context)->stream->item((context)->stream))
#define next(context) ((context)->stream->next((context)->stream))

static void skip_blanks(struct json_parse_context *context) {
     int c = item(context);
     while (c == ' ' || c == '\f' || c == '\t' || c == '\n' || c == '\r') {
          next(context);
          c = item(context);
     }
}

static int skip_word(struct json_parse_context *context, const char *word) {
     int result = 1;
     while (result && *word) {
          result = *word == item(context);
          next(context);
     }
     return result;
}

static char *utf8(struct json_parse_context *context, json_string_t *string) {
     char *result = context->utf8_buffer;
     int capacity = context->utf8_capacity;
     int n = string->utf8(string, result, capacity);
     if (n >= capacity) {
          do {
               capacity <<= 1;
          } while (n >= capacity);
          result = context->memory.malloc(capacity);
          strcpy(result, context->utf8_buffer);
          context->memory.free(context->utf8_buffer);
          context->utf8_buffer = result;
          context->utf8_capacity = capacity;
          string->utf8(string, result, capacity);
     }
     return result;
}

__PUBLIC__ json_value_t *json_parse(json_input_stream_t *stream, json_on_error_fn on_error, json_memory_t memory) {
     struct json_parse_context _context = {
          stream,
          on_error ? on_error : &default_on_error,
          1,
          1,
          memory,
          memory.malloc(128),
          128,
     };
     struct json_parse_context *context = &_context;
     json_value_t *result = parse_value(context);
     skip_blanks(context);
     if (item(context) != -1) {
          error(context, "Trailing characters", 0);
     }
     memory.free(_context.utf8_buffer);
     return result;
}

static json_value_t *parse_value(struct json_parse_context *context) {
     json_value_t *result = NULL;
     skip_blanks(context);
     switch(item(context)) {
     case '{':
          result = (json_value_t*)parse_object(context);
          break;
     case '[':
          result = (json_value_t*)parse_array(context);
          break;
     case '"':
          result = (json_value_t*)parse_string(context);
          break;
     case 't':
          result = (json_value_t*)parse_true(context);
          break;
     case 'f':
          result = (json_value_t*)parse_false(context);
          break;
     case 'n':
          result = (json_value_t*)parse_null(context);
          break;
     case '0':
     case '1':
     case '2':
     case '3':
     case '4':
     case '5':
     case '6':
     case '7':
     case '8':
     case '9':
     case '-':
          result = (json_value_t*)parse_number(context);
          break;
     case -1:
          /* end of stream */
          break;
     default:
          error(context, "Invalid character '%c'", item(context));
     }
     return result;
}

static json_object_t *parse_object(struct json_parse_context *context) {
     json_object_t *result = json_new_object(context->memory);
     json_string_t *key;
     json_value_t  *value;

     int done = 0, err = 0;
     next(context);
     while (!done && !err) {
          skip_blanks(context);
          if (item(context) != '"') {
               err = 1;
               error(context, "Expected string", 0);
          }
          else {
               key = parse_string(context);
               if (key) {
                    if (result->get(result, utf8(context, key))) {
                         err = 1;
                         error(context, "Duplicate key: '%s'", utf8(context, key));
                    }
                    else {
                         skip_blanks(context);
                         if (item(context) != ':') {
                              err = 1;
                              error(context, "Expected ':'", 0);
                         }
                         next(context);
                         value = parse_value(context);
                         if (!value) {
                              err = 1;
                         }
                         else {
                              result->set(result, utf8(context, key), value);
                              key->free(key);
                              skip_blanks(context);
                              switch(item(context)) {
                              case '}':
                                   next(context);
                                   done = 1;
                                   break;
                              case ',':
                                   next(context);
                                   break;
                              default:
                                   err = 1;
                                   error(context, "Expected ',' or '}'", 0);
                              }
                         }
                    }
               }
          }
     }
     return result;
}

static json_array_t *parse_array(struct json_parse_context *context) {
     json_array_t *result = json_new_array(context->memory);
     json_value_t  *value;

     int done = 0, err = 0;
     next(context);
     while (!done && !err) {
          skip_blanks(context);
          value = parse_value(context);
          if (!value) {
               err = 1;
          }
          else {
               result->add(result, value);
               skip_blanks(context);
               switch(item(context)) {
               case ']':
                    next(context);
                    done = 1;
                    break;
               case ',':
                    next(context);
                    break;
               default:
                    err = 1;
                    error(context, "Expected ',' or ']'", 0);
               }
          }
     }
     return result;
}

#define NUM_STATE_ERROR              -2
#define NUM_STATE_DONE               -1
#define NUM_STATE_INTEGRAL            0
#define NUM_STATE_DECIMAL_FIRST      10
#define NUM_STATE_DECIMAL_MORE       11
#define NUM_STATE_EXP_SIGN_OR_FIRST  20
#define NUM_STATE_EXP_FIRST          21
#define NUM_STATE_EXP_MORE           22

static json_number_t *parse_number(struct json_parse_context *context) {
     json_number_t * result = NULL;
     int state, i=0, d=0, dx=0, x=0, n=1, nx=1;
     if (item(context) == '-') {
          n = -1;
          next(context);
     }
     switch(item(context)) {
     case '0':
          state = NUM_STATE_DONE;
          next(context);
          break;
     case '1': case '2': case '3':
     case '4': case '5': case '6':
     case '7': case '8': case '9':
          i = item(context) - '0';
          state = NUM_STATE_INTEGRAL;
          next(context);
          break;
     default:
          state = NUM_STATE_ERROR;
          error(context, "Invalid number", 0);
     }

     while (state >= 0) {
          int c = item(context);
          if (c < 0) {
               state = NUM_STATE_ERROR;
               error(context, "Invalid number", 0);
          }
          else {
               switch(state) {

               case NUM_STATE_INTEGRAL           :
                    switch(c) {
                    case '.':
                         state = NUM_STATE_DECIMAL_FIRST;
                         next(context);
                         break;
                    case 'e': case 'E':
                         state = NUM_STATE_EXP_SIGN_OR_FIRST;
                         next(context);
                         break;
                    case '0':
                    case '1': case '2': case '3':
                    case '4': case '5': case '6':
                    case '7': case '8': case '9':
                         i = i * 10 + c - '0';
                         state = NUM_STATE_INTEGRAL;
                         next(context);
                         break;
                    default:
                         state = NUM_STATE_DONE;
                    }
                    break;

               case NUM_STATE_DECIMAL_FIRST      :
                    switch(c) {
                    case '0':
                    case '1': case '2': case '3':
                    case '4': case '5': case '6':
                    case '7': case '8': case '9':
                         d = c - '0';
                         dx = 1;
                         state = NUM_STATE_DECIMAL_MORE;
                         next(context);
                         break;
                    default:
                         state = NUM_STATE_ERROR;
                         error(context, "Invalid number", 0);
                    }
                    break;

               case NUM_STATE_DECIMAL_MORE       :
                    switch(c) {
                    case 'e': case 'E':
                         state = NUM_STATE_EXP_SIGN_OR_FIRST;
                         next(context);
                    case '0':
                    case '1': case '2': case '3':
                    case '4': case '5': case '6':
                    case '7': case '8': case '9':
                         d = d * 10 + c - '0';
                         dx++;
                         state = NUM_STATE_DECIMAL_MORE;
                         next(context);
                         break;
                    default:
                         state = NUM_STATE_DONE;
                    }
                    break;

               case NUM_STATE_EXP_SIGN_OR_FIRST  :
                    switch(c) {
                    case '+':
                         state = NUM_STATE_EXP_FIRST;
                         next(context);
                         break;
                    case '-':
                         nx = -1;
                         state = NUM_STATE_EXP_FIRST;
                         next(context);
                         break;
                    case '0':
                    case '1': case '2': case '3':
                    case '4': case '5': case '6':
                    case '7': case '8': case '9':
                         x = c - '0';
                         state = NUM_STATE_EXP_MORE;
                         next(context);
                         break;
                    default:
                         state = NUM_STATE_ERROR;
                         error(context, "Invalid number", 0);
                    }
                    break;

               case NUM_STATE_EXP_FIRST          :
                    switch(c) {
                    case '0':
                    case '1': case '2': case '3':
                    case '4': case '5': case '6':
                    case '7': case '8': case '9':
                         x = c - '0';
                         state = NUM_STATE_EXP_MORE;
                         next(context);
                         break;
                    default:
                         state = NUM_STATE_ERROR;
                         error(context, "Invalid number", 0);
                    }
                    break;

               case NUM_STATE_EXP_MORE           :
                    switch(c) {
                    case '0':
                    case '1': case '2': case '3':
                    case '4': case '5': case '6':
                    case '7': case '8': case '9':
                         x = x * 10 + c - '0';
                         state = NUM_STATE_EXP_MORE;
                         next(context);
                         break;
                    default:
                         state = NUM_STATE_DONE;
                    }
                    break;
               }
          }
     }

     if (state == NUM_STATE_DONE) {
          result = json_new_number(context->memory);
          result->set(result, n*i, d, dx, nx*x);
     }
     return result;
}

#define STR_STATE_ERROR    -2
#define STR_STATE_DONE     -1
#define STR_STATE_CHAR      0
#define STR_STATE_ESCAPE    1
#define STR_STATE_UNICODE0 10
#define STR_STATE_UNICODE1 11
#define STR_STATE_UNICODE2 12
#define STR_STATE_UNICODE3 13

/* TODO: fix unicode support (here totally wrong) + correct support for character appending */

static json_string_t *parse_string(struct json_parse_context *context) {
     int state, unicode;
     json_string_t *result = json_new_string(context->memory);

     next(context); // skip '"'
     state = STR_STATE_CHAR;
     while (state >= 0) {
          int c = item(context);
          if (c < 0) {
               state = STR_STATE_ERROR;
               error(context, "Invalid string", 0);
          }
          else {
               switch(state) {

               case STR_STATE_CHAR:
                    switch(c) {
                    case '\\':
                         state = STR_STATE_ESCAPE;
                         break;
                    case '"':
                         state = STR_STATE_DONE;
                         break;
                    default:
                         result->add_utf8(result, c);
                    }
                    break;

               case STR_STATE_ESCAPE:
                    switch(c) {
                    case '"': case '\\':
                         result->add(result, c);
                         break;
                    case 'b':
                         result->add(result, '\b');
                         break;
                    case 'f':
                         result->add(result, '\f');
                         break;
                    case 'n':
                         result->add(result, '\n');
                         break;
                    case 'r':
                         result->add(result, '\r');
                         break;
                    case 't':
                         result->add(result, '\t');
                         break;
                    case 'u':
                         state = STR_STATE_UNICODE0;
                         unicode = 0;
                         break;
                    default:
                         state = STR_STATE_ERROR;
                         error(context, "Invalid escape sequence", 0);
                    }
                    break;

               case STR_STATE_UNICODE0: case STR_STATE_UNICODE1:
               case STR_STATE_UNICODE2: case STR_STATE_UNICODE3:
                    switch(c) {
                    case '0':
                    case '1': case '2': case '3':
                    case '4': case '5': case '6':
                    case '7': case '8': case '9':
                         unicode = unicode * 16 + c - '0';
                         break;
                    case 'a': case 'b': case 'c':
                    case 'd': case 'e': case 'f':
                         unicode = unicode * 16 + c - 'a' + 10;
                         break;
                    case 'A': case 'B': case 'C':
                    case 'D': case 'E': case 'F':
                         unicode = unicode * 16 + c - 'A' + 10;
                         break;
                    default:
                         state = STR_STATE_ERROR;
                         error(context, "Invalid unicode sequence: expected 4 hex, got only %d", state - STR_STATE_UNICODE0);
                    }
                    if (state == STR_STATE_UNICODE3) {
                         result->add(result, unicode);
                         state = STR_STATE_CHAR;
                    }
                    else {
                         state++;
                    }
                    break;
               }
               next(context);
          }
     }

     if (state == STR_STATE_ERROR) {
          result->free(result);
          result = NULL;
     }
     return result;
}

static json_const_t *parse_true(struct json_parse_context *context) {
     if (skip_word(context, "true")) {
          return json_const(json_true);
     }
     error(context, "Expected 'true'", 0);
     return NULL;
}

static json_const_t *parse_false(struct json_parse_context *context) {
     if (skip_word(context, "false")) {
          return json_const(json_false);
     }
     error(context, "Expected 'false'", 0);
     return NULL;
}

static json_const_t *parse_null(struct json_parse_context *context) {
     if (skip_word(context, "null")) {
          return json_const(json_null);
     }
     error(context, "Expected 'null'", 0);
     return NULL;
}
