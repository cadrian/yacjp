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

typedef struct json_value   json_value_t;

typedef struct json_object  json_object_t;
typedef struct json_array   json_array_t;
typedef struct json_string  json_string_t;
typedef struct json_number  json_number_t;

typedef struct json_visitor json_visitor_t;


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

typedef struct json_object_field json_object_field_t;
struct json_object_field {
   char *key;
   json_value_t *value;
};

typedef void                 (*json_object_accept_fn   ) (json_object_t *this);
typedef unsigned int         (*json_object_count_fn    ) (json_object_t *this, json_visitor_t *visitor);
typedef json_object_field_t  (*json_object_get_field_fn) (json_object_t *this, int i);
typedef json_value_t        *(*json_object_get_value_fn) (json_object_t *this, char *key);
typedef void                 (*json_object_set_value_fn) (json_object_t *this, char *key, json_value_t *value);
typedef void                 (*json_object_del_value_fn) (json_object_t *this, char *key);

struct json_object {
   json_object_accept_fn    accept   ;
   json_object_count_fn     count    ;
   json_object_get_field_fn get_field;
   json_object_get_value_fn get_value;
   json_object_set_value_fn set_value;
   json_object_del_value_fn del_value;
};

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

typedef void          (*json_array_accept_fn) (json_array_t *this, json_visitor_t *visitor);
typedef unsigned int  (*json_array_count_fn ) (json_array_t *this);
typedef json_value_t *(*json_array_get_fn   ) (json_array_t *this, int i);
typedef void          (*json_array_set_fn   ) (json_array_t *this, int i, json_value_t *value);
typedef void          (*json_array_add_fn   ) (json_array_t *this,        json_value_t *value);
typedef void          (*json_array_del_fn   ) (json_array_t *this, int i);

struct json_array {
   json_array_accept_fn accept;
   json_array_count_fn  count ;
   json_array_get_fn    get   ;
   json_array_set_fn    set   ;
   json_array_add_fn    add   ;
   json_array_del_fn    del   ;
};

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

typedef void  (*json_string_accept_fn) (json_string_t *this, json_visitor_t *visitor);
typedef char *(*json_string_get_fn   ) (json_string_t *string);
typedef void  (*json_string_set_fn   ) (json_string_t *string, char *format, ...);

struct json_string {
   json_string_accept_fn accept;
   json_string_get_fn    get   ;
   json_string_set_fn    set   ;
};

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

typedef void   (*json_number_accept_fn   ) (json_number_t *this, json_visitor_t *visitor);
typedef int    (*json_number_is_int_fn   ) (json_number_t *this);
typedef int    (*json_number_to_int_fn   ) (json_number_t *this);
typedef double (*json_number_to_double_fn) (json_number_t *this);
typedef void   (*json_number_set_fn      ) (json_number_t *this, int integral, int decimal, int decimal_exp, int exp);

struct json_number {
   json_number_accept_fn    accept   ;
   json_number_is_int_fn    is_int   ;
   json_number_to_int_fn    to_int   ;
   json_number_to_double_fn to_double;
   json_number_set_fn       set      ;
};

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

struct json_value {
   void (*accept) (json_value_t *this, json_visitor_t *visitor);
};

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

struct json_visitor {
   void (*visit_object) (json_visitor_t *this, json_object_t *visited);
   void (*visit_array)  (json_visitor_t *this, json_array_t  *visited);
   void (*visit_string) (json_visitor_t *this, json_string_t *visited);
   void (*visit_number) (json_visitor_t *this, json_number_t *visited);
};

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

__PUBLIC__ json_value_t *json_new_object();
__PUBLIC__ json_value_t *json_new_array();
__PUBLIC__ json_value_t *json_new_string();
__PUBLIC__ json_value_t *json_new_number();

#endif /* _YACJP_JSON_VALUE_H_ */
