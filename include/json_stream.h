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

typedef int (*next_fn)(json_input_stream_t *this);
typedef int (*item_fn)(json_input_stream_t *this);

struct json_input_stream {
   next_fn next;
   item_fn item;
};

__PUBLIC__ json_input_stream_t *new_json_input_stream_from_string          (char *string, json_memory_t memory);
__PUBLIC__ json_input_stream_t *new_json_input_stream_from_file            (FILE *file,   json_memory_t memory);
__PUBLIC__ json_input_stream_t *new_json_input_stream_from_file_description(int fd,       json_memory_t memory);

#endif /* _YACJP_JSON_STREAM_H_ */
