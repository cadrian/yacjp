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

#include <stdarg.h>

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "test.h"
#include "json.h"

static cad_input_stream_t *stream;

static void on_error(cad_input_stream_t *s, int line, int column, void *data, const char *format, ...) {
     char *a = 0;
     va_list args;
     assert(stream == s);
     va_start(args, format);
     vfprintf(stderr, format, args);
     va_end(args);
     *a=0;
}

int main() {
     json_value_t *value;
     int fd = open("target/out/data/config.ini", O_RDONLY);

     stream = new_cad_input_stream_from_file_descriptor(fd, stdlib_memory);
     value = json_parse(stream, on_error, NULL, stdlib_memory);
     close(fd);

     assert(value != NULL);

     return 0;
}
