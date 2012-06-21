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

#include <stdio.h>
#include <stdlib.h>

static void assert_(int test, const char *message, int line, const char *file) {
     int *a = 0;
     static int counter = 0;
     counter++;
     if (!test) {
          fprintf(stderr, "Assertion %d (%s:%d) failed: %s\n", counter, file, line, message);
          fflush(stderr);
          *a = counter;
     }
}

#define assert(t) assert_((t), #t, __LINE__, __FILE__)
