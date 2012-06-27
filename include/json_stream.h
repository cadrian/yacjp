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
 * @addtogroup json_stream
 * @{
 */

/**
 * @addtogroup json_in_stream
 * @{
 */

/**
 * The JSON input stream public interface.
 *
 * The abstraction is a data cursor: the "current" byte is always
 * available via item(); use next() to advance the cursor.
 */
typedef struct json_input_stream json_input_stream_t;

/**
 * Free the input stream
 *
 * @param[in] this the target input stream
 */
typedef void (*json_input_stream_free_fn)(json_input_stream_t *this);

/**
 * Ask the JSON stream to read the next byte.
 *
 * @param[in] this the target JSON input stream
 *
 * @return 0 if OK, -1 if error
 */
typedef int (*json_input_stream_next_fn)(json_input_stream_t *this);

/**
 * Ask the JSON input stream the last read byte.
 *
 * @param[in] this the target JSON input stream
 *
 * @return the current byte in the stream.
 */
typedef int (*json_input_stream_item_fn)(json_input_stream_t *this);

struct json_input_stream {
     /**
      * @see json_input_stream_free_fn
      */
     json_input_stream_free_fn free;
     /**
      * @see json_input_stream_next_fn
      */
     json_input_stream_next_fn next;

     /**
      * @see json_input_stream_item_fn
      */
     json_input_stream_item_fn item;
};

/**
 * Create a new JSON input stream that reads bytes from the given C
 * string (will stop at '\\0'), using the given memory manager and
 * returns it.
 *
 * @param[in] string the C string to read bytes from
 * @param[in] memory the memory manager
 *
 * @return a stream that reads bytes from the given string.
 */
__PUBLIC__ json_input_stream_t *new_json_input_stream_from_string         (char *string, json_memory_t memory);

/**
 * Create a new JSON input stream that reads bytes from the given
 * file, using the given memory manager and returns it.
 *
 * @param[in] file the file to read from (must be open for reading)
 * @param[in] memory the memory manager
 *
 * @return a stream that reads bytes from the given file.
 */
__PUBLIC__ json_input_stream_t *new_json_input_stream_from_file           (FILE *file,   json_memory_t memory);

/**
 * Create a new JSON input stream that reads bytes from the given
 * file descriptor, using the given memory manager and returns it.
 *
 * @param[in] fd the file descriptor to read from (must be open for reading)
 * @param[in] memory the memory manager
 *
 * @return a stream that reads bytes from the given file descriptor.
 */
__PUBLIC__ json_input_stream_t *new_json_input_stream_from_file_descriptor(int fd,       json_memory_t memory);

/**
 * @}
 */

/**
 * @addtogroup json_out_stream
 * @{
 */

/**
 * The JSON output stream interface.
 *
 * The abstraction is a byte bucket.
 */
typedef struct json_output_stream json_output_stream_t;

/**
 * Free the output stream
 *
 * @param[in] this the target output stream
 */
typedef void (*json_output_stream_free_fn)(json_output_stream_t *this);

/**
 * Put bytes to the output stream
 *
 * @param[in] this the target JSON output stream
 * @param[in] format the format of the bytes to put, compatible with printf()
 * @param[in] ... the arguments of the format
 */
typedef void (*json_output_stream_put_fn  )(json_output_stream_t *this, const char *format, ...) __PRINTF__;

/**
 * Flush bytes to the underlying stream
 *
 * @param[in] this the target JSON output stream
 */
typedef void (*json_output_stream_flush_fn)(json_output_stream_t *this);

struct json_output_stream {
     /**
      * @see json_output_stream_free_fn
      */
     json_output_stream_free_fn  free ;
     /**
      * @see json_output_stream_put_fn
      */
     json_output_stream_put_fn   put  ;
     /**
      * @see json_output_stream_flush_fn
      */
     json_output_stream_flush_fn flush;
};

/**
 * Create a new JSON output stream using the memory manager and
 * returns it. That string is allocated as needed (and may be
 * allocated more than once); the address of the string is always
 * stored at the provided `string` location.
 *
 * @param[in,out] string the address of the string
 * @param[in] memory the memory manager
 *
 * @return a stream that writes bytes to a string.
 */
__PUBLIC__ json_output_stream_t *new_json_output_stream_from_string         (char **string, json_memory_t memory);

/**
 * Create a new JSON output stream using the memory manager and
 * returns it.
 *
 * @param[in] file the file to write bytes to (must be open for writing or appending)
 * @param[in] memory the memory manager
 *
 * @return a stream that writes bytes into the given file.
 */
__PUBLIC__ json_output_stream_t *new_json_output_stream_from_file           (FILE *file,    json_memory_t memory);

/**
 * Create a new JSON output stream using the memory manager and
 * returns it.
 *
 * @param[in] fd the file descriptor to write bytes to (must be open for writing or appending)
 * @param[in] memory the memory manager
 *
 * @return a stream that writes bytes into the given file descriptor.
 */
__PUBLIC__ json_output_stream_t *new_json_output_stream_from_file_descriptor(int fd,        json_memory_t memory);

/**
 * @}
 */

/**
 * @}
 */

#endif /* _YACJP_JSON_STREAM_H_ */
