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

#include "json.h"

static void assert_(int test, const char *message, int line, const char *file) {
     static int counter = 0;
     counter++;
     if (!test) {
          fprintf(stderr, "Assertion %d (%s:%d) failed: %s\n", counter, file, line, message);
          exit(counter);
     }
}

#define assert(t) assert_((t), #t, __LINE__, __FILE__)

int main() {
     json_object_t *value = json_new_object(stdlib_memory);
     json_value_t *v;
     json_value_t *t = (json_value_t*)json_const(json_true);
     json_value_t *f = (json_value_t*)json_const(json_false);
     json_value_t *n = (json_value_t*)json_const(json_null);
     const char *keys[2];

     assert(value->count(value) == 0);

     value->set(value, "foo", t);
     assert(value->count(value) == 1);
     v = value->get(value, "foo");
     assert(v == t);
     keys[0] = keys[1] = NULL;
     value->keys(value, keys);
     assert(!strcmp("foo", keys[0]));
     assert(keys[1] == NULL);

     value->set(value, "bar", f);
     assert(value->count(value) == 2);
     v = value->get(value, "bar");
     assert(v == f);
     v = value->get(value, "foo");
     assert(v == t);
     keys[0] = keys[1] = NULL;
     value->keys(value, keys);
     assert(!strcmp("foo", keys[0]));
     assert(!strcmp("bar", keys[1]));

     value->set(value, "foo", n);
     assert(value->count(value) == 2);
     v = value->get(value, "bar");
     assert(v == f);
     v = value->get(value, "foo");
     assert(v == n);
     keys[0] = keys[1] = NULL;
     value->keys(value, keys);
     assert(!strcmp("foo", keys[0]));
     assert(!strcmp("bar", keys[1]));

     value->del(value, "foo");
     assert(value->count(value) == 1);
     v = value->get(value, "bar");
     assert(v == f);
     keys[0] = keys[1] = NULL;
     value->keys(value, keys);
     assert(!strcmp("bar", keys[0]));
     assert(keys[1] == NULL);

     value->accept(value, json_kill());

     return 0;
}
