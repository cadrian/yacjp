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
 * The JSON file to include.
 */

#include "json_shared.h"
#include "json_stream.h"
#include "json_value.h"

/**
 * This function is called by the JSON parser if an error is met while
 * parsing a JSON stream.
 */
typedef void (*json_on_error_fn)(json_input_stream_t *stream, int line, int column, const char *format, ...);

/**
 * Parse a JSON stream.
 *
 * @return the parsed JSON value, or NULL if an error occured (in the
 * latter case, the on_error function was also called).
 */
__PUBLIC__ json_value_t *json_parse(json_input_stream_t *stream, json_on_error_fn on_error, json_memory_t memory);

/**
 * Kill a JSON value: recursively free()s all the values contained in
 * objects and arrays.
 */
__PUBLIC__ json_visitor_t *json_kill();

/**
 * An argument to write_to() to obtain a compact JSON output.
 */
__PUBLIC__ extern short json_compact;

/**
 * An argument to write_to() to obtain '\u' sequences instead of raw
 * utf-8.
 */
__PUBLIC__ extern short json_extend_unicode; /* use '\u' sequences instead of raw utf8 */

/**
 * An argument to write_to() to obtain a pretty output with spaces and newlines.
 */
__PUBLIC__ extern short json_extend_spaces;  /* pretty print spaces, add newlines when relevant */

/**
 * @return a visitor that is able to write a JSON value to the given stream.
 *
 * Sensible options are json_compact, json_extend_unicode,
 * json_extend_spaces, or json_extend_unicode || json_extend_spaces
 */
__PUBLIC__ json_visitor_t *json_write_to(json_output_stream_t *stream, json_memory_t memory, short options);

#endif /* _YACJP_JSON_H_ */
