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

#ifndef _YACJP_JSON_VALUE_H_
#define _YACJP_JSON_VALUE_H_

#include "json_shared.h"

typedef struct json_value json_value_t;
typedef struct json_visitor json_visitor_t;

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

typedef struct json_object_field json_object_field_t;
struct json_object_field {
   char *key;
   json_value_t *value;
};

typedef struct json_object {
   unsigned int         count    (json_object_t *this);
   json_object_field_t  get_field(json_object_t *this, int i);
   json_value_t        *get_value(json_object_t *this, char *key);
   void                 set_value(json_object_t *this, char *key, json_value_t *value);
   void                 del_value(json_object_t *this, char *key);
} json_object_t;

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

typedef struct json_array {
   unsigned int  count(json_array_t *this);
   json_value_t *get  (json_array_t *this, int i);
   void          set  (json_array_t *this, int i, json_value_t *value);
   void          add  (json_array_t *this,        json_value_t *value);
   void          del  (json_array_t *this, int i);
} json_array_t;

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

typedef struct json_string {
   char *get(json_string_t *string);
   void  set(json_string_t *string, char *format, ...);
} json_string_t;

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

typedef struct json_number {
   int    is_int   (json_number_t *this);
   int    to_int   (json_number_t *this);
   double to_double(json_number_t *this);
   void   set      (json_number_t *this, int integral, int decimal, int decimal_exp, int exp);
} json_number_t;

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

struct json_value {
   void (*accept)(json_value_t *this);

   union {
      json_object_t object;
      json_array_t  array;
      json_string_t string;
      json_number_t number;
   } value;
};

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

struct json_visitor {
   void (*visit_object) (json_visitor_t *this, json_object_t *visited);
   void (*visit_array)  (json_visitor_t *this, json_array_t  *visited);
   void (*visit_string) (json_visitor_t *this, json_string_t *visited);
   void (*visit_number) (json_visitor_t *this, json_number_t *visited);
};

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

json_object_t *json_new_object();
json_array_t  *json_new_array();
json_string_t *json_new_string();
json_number_t *json_new_number();

#endif /* _YACJP_JSON_VALUE_H_ */
