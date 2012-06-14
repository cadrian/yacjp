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

#include "json_stream.h"
#include "json_value.h"
#include "json_shared.h"

__PUBLIC__ json_value_t *json_parse(json_stream_t *stream);

#endif /* _YACJP_JSON_H_ */
