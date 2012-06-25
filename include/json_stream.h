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

/**
 * @file
 * The JSON stream subsystem.
 */

#include <stdio.h>

#include "json_shared.h"

/**
 * @ingroup json_stream
 * @{
 */

typedef struct json_input_stream json_input_stream_t;

typedef int (*json_input_stream_next_fn)(json_input_stream_t *this);
typedef int (*json_input_stream_item_fn)(json_input_stream_t *this);

/**
 * The JSON input stream public interface.
 *
 * The abstraction is a data cursor: the "current" byte is always
 * available via item(); use next() to advance the cursor.
 */
struct json_input_stream {
     /**
      * Advances one byte in the stream.
      *
      * @return 0 when ok, -1 if an error occurred.
      */
     json_input_stream_next_fn next;

     /**
      * @return the current byte in the stream.
      */
     json_input_stream_item_fn item;
};

/**
 * @return a stream that reads bytes from the given string.
 */
__PUBLIC__ json_input_stream_t *new_json_input_stream_from_string         (char *string, json_memory_t memory);

/**
 * @return a stream that reads bytes from the given file.
 */
__PUBLIC__ json_input_stream_t *new_json_input_stream_from_file           (FILE *file,   json_memory_t memory);

/**
 * @return a stream that reads bytes from the given file descriptor.
 */
__PUBLIC__ json_input_stream_t *new_json_input_stream_from_file_descriptor(int fd,       json_memory_t memory);


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


typedef struct json_output_stream json_output_stream_t;

typedef void (*json_output_stream_put_fn  )(json_output_stream_t *this, const char *format, ...) __attribute__((format(printf, 2, 3)));
typedef void (*json_output_stream_flush_fn)(json_output_stream_t *this);

/**
 * The JSON output stream interface.
 *
 * The abstraction is a byte bucket.
 */
struct json_output_stream {
     json_output_stream_put_fn   put;
     json_output_stream_flush_fn flush;
};

/**
 * @return a stream that writes bytes to a string. That string is
 * allocated as needed (and may be allocated more than once); the
 * address of the string is always stored at the provided location.
 */
__PUBLIC__ json_output_stream_t *new_json_output_stream_from_string         (char **string, json_memory_t memory);

/**
 * @return a stream that writes bytes into the given file.
 */
__PUBLIC__ json_output_stream_t *new_json_output_stream_from_file           (FILE *file,    json_memory_t memory);

/**
 * @return a stream that writes bytes into the given file descriptor.
 *
 * Note: not implemented.
 */
__PUBLIC__ json_output_stream_t *new_json_output_stream_from_file_descriptor(int fd,        json_memory_t memory);

/**
 * @}
 */

#endif /* _YACJP_JSON_STREAM_H_ */
