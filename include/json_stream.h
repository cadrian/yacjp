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
#include <cad_shared.h>
#include <cad_stream.h>

/**
 * @addtogroup json_unicode_stream
 * @{
 */

/**
 * Creates and returns a stream that provides utf8-encoded data based
 * on the actual encoding type (determined by reading the first raw
 * bytes as per RFC4627)
 *
 * @return the UTF-8 input stream
 */
__PUBLIC__ cad_input_stream_t *new_json_utf8_stream(cad_input_stream_t *raw, cad_memory_t memory);

/**
 * @}
 */

#endif /* _YACJP_JSON_STREAM_H_ */
