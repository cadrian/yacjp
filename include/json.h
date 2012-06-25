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

#ifndef _YACJP_JSON_H_
#define _YACJP_JSON_H_

/**
 * @file
 * The JSON file to include.
 *
 * This header provides all the necessary definitions to use the library.
 */

#include "json_shared.h"
#include "json_stream.h"
#include "json_value.h"

/**
 * @ingroup json_parse
 * @{
 */

/**
 * This function is called by the JSON parser if an error is met while
 * parsing a JSON stream.
 *
 * @param[in] stream the input stream that was parsed when the error occurred
 * @param[in] line the line number of the error
 * @param[in] column the column number of the error
 * @param[in] format the format of the error message, printf()-compliant
 * @param[in] ... the arguments of the format
 */
typedef void (*json_on_error_fn)(json_input_stream_t *stream, int line, int column, const char *format, ...);

/**
 * Parse a JSON stream.
 *
 * @param[in] stream the stream that contains the JSON data to parse
 * @param[in] on_error the function to call if a parse error occurs
 * @param[in] memory the memory manager that will allocate memory for the parsed JSON objects
 *
 * @return the parsed JSON value, or NULL if an error occured (in the
 * latter case, the on_error function was also called).
 */
__PUBLIC__ json_value_t *json_parse(json_input_stream_t *stream, json_on_error_fn on_error, json_memory_t memory);

/**
 * @}
 */

/**
 * @ingroup json_utils
 * @{
 */

/**
 * This visitor kills a JSON value: recursively free()s all the values
 * contained in objects and arrays.
 *
 * @return a singleton object able to recursively kill any JSON value.
 */
__PUBLIC__ json_visitor_t *json_kill();

/**
 * @}
 */

/**
 * @ingroup json_write
 * @{
 */

/**
 * An argument to json_write_to() to obtain a compact JSON output.
 *
 * @see json_write_to
 */
__PUBLIC__ extern short json_compact;

/**
 * An argument to json_write_to() to obtain '\\u' sequences instead of
 * raw utf-8.
 *
 * @see json_write_to
 */
__PUBLIC__ extern short json_extend_unicode;

/**
 * An argument to json_write_to() to obtain a pretty output with
 * spaces and newlines.
 *
 * @see json_write_to
 */
__PUBLIC__ extern short json_extend_spaces;

/**
 * Builds a visitor that can write any given JSON data to the `stream`.
 *
 * @param[in] stream the stream onto which the JSON data will be written
 * @param[in] memory the memory manager that will allocate memory if needed
 * @param[in] options Sensible options are @ref json_compact, @ref
 * json_extend_unicode, @ref json_extend_spaces, or the combined @ref
 * json_extend_unicode || @ref json_extend_spaces.
 *
 * @return a visitor that is able to write a JSON value to the given
 * `stream`.
 */
__PUBLIC__ json_visitor_t *json_write_to(json_output_stream_t *stream, json_memory_t memory, short options);

/**
 * @}
 */

#endif /* _YACJP_JSON_H_ */
