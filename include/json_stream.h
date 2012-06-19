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

#ifndef _YACJP_JSON_STREAM_H_
#define _YACJP_JSON_STREAM_H_

#include <stdio.h>

#include "json_shared.h"

typedef struct json_input_stream json_input_stream_t;
typedef struct json_output_stream json_output_stream_t;


typedef int (*json_input_stream_next_fn)(json_input_stream_t *this);
typedef int (*json_input_stream_item_fn)(json_input_stream_t *this);

struct json_input_stream {
   json_input_stream_next_fn next;
   json_input_stream_item_fn item;
};

__PUBLIC__ json_input_stream_t *new_json_input_stream_from_string         (char *string, json_memory_t memory);
__PUBLIC__ json_input_stream_t *new_json_input_stream_from_file           (FILE *file,   json_memory_t memory);
__PUBLIC__ json_input_stream_t *new_json_input_stream_from_file_descriptor(int fd,       json_memory_t memory);


typedef void (*json_output_stream_put_fn  )(json_output_stream_t *this, const char *format, ...) __attribute__((format(printf, 2, 3)));
typedef void (*json_output_stream_flush_fn)(json_output_stream_t *this);

struct json_output_stream {
     json_output_stream_put_fn   put;
     json_output_stream_flush_fn flush;
};

__PUBLIC__ json_output_stream_t *new_json_output_stream_from_string         (char **string, json_memory_t memory);
__PUBLIC__ json_output_stream_t *new_json_output_stream_from_file           (FILE *file,    json_memory_t memory);
__PUBLIC__ json_output_stream_t *new_json_output_stream_from_file_descriptor(int fd,        json_memory_t memory);

#endif /* _YACJP_JSON_STREAM_H_ */
