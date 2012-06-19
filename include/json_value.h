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
typedef struct json_const   json_const_t;

typedef struct json_visitor json_visitor_t;


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

typedef struct json_object_field json_object_field_t;
struct json_object_field {
     const char *key;
     json_value_t *value;
};

typedef void                 (*json_object_accept_fn) (json_object_t *this, json_visitor_t *visitor);
typedef void                 (*json_object_free_fn  ) (json_object_t *this);
typedef unsigned int         (*json_object_count_fn ) (json_object_t *this);
typedef void                 (*json_object_keys_fn  ) (json_object_t *this, const char **keys);
typedef json_value_t        *(*json_object_get_fn   ) (json_object_t *this, const char *key);
typedef json_value_t        *(*json_object_set_fn   ) (json_object_t *this, const char *key, json_value_t *value);
typedef json_value_t        *(*json_object_del_fn   ) (json_object_t *this, const char *key);

struct json_object {
     json_object_accept_fn accept;
     json_object_free_fn   free  ;
     json_object_count_fn  count ;
     json_object_keys_fn   keys  ;
     json_object_get_fn    get   ;
     json_object_set_fn    set   ;
     json_object_del_fn    del   ;
};

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

typedef void          (*json_array_accept_fn) (json_array_t *this, json_visitor_t *visitor);
typedef void          (*json_array_free_fn  ) (json_array_t *this);
typedef unsigned int  (*json_array_count_fn ) (json_array_t *this);
typedef json_value_t *(*json_array_get_fn   ) (json_array_t *this, int index);
typedef void          (*json_array_set_fn   ) (json_array_t *this, int index, json_value_t *value);
typedef void          (*json_array_ins_fn   ) (json_array_t *this, int index, json_value_t *value);
typedef void          (*json_array_add_fn   ) (json_array_t *this,            json_value_t *value);
typedef void          (*json_array_del_fn   ) (json_array_t *this, int index);

struct json_array {
     json_array_accept_fn accept;
     json_array_free_fn   free  ;
     json_array_count_fn  count ;
     json_array_get_fn    get   ;
     json_array_set_fn    set   ;
     json_array_set_fn    ins   ;
     json_array_add_fn    add   ;
     json_array_del_fn    del   ;
};

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

typedef void  (*json_string_accept_fn) (json_string_t *this, json_visitor_t *visitor);
typedef void  (*json_string_free_fn  ) (json_string_t *this);
typedef char *(*json_string_get_fn   ) (json_string_t *this);
typedef void  (*json_string_set_fn   ) (json_string_t *this, char *format, ...) __attribute__((format(printf, 2, 3)));

struct json_string {
     json_string_accept_fn accept;
     json_string_free_fn   free  ;
     json_string_get_fn    get   ;
     json_string_set_fn    set   ;
};

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

typedef void   (*json_number_accept_fn   ) (json_number_t *this, json_visitor_t *visitor);
typedef void   (*json_number_free_fn     ) (json_number_t *this);
typedef int    (*json_number_is_int_fn   ) (json_number_t *this);
typedef int    (*json_number_to_int_fn   ) (json_number_t *this);
typedef double (*json_number_to_double_fn) (json_number_t *this);
typedef void   (*json_number_set_fn      ) (json_number_t *this, int integral, int decimal, int decimal_exp, int exp);

struct json_number {
     json_number_accept_fn    accept   ;
     json_number_free_fn      free     ;
     json_number_is_int_fn    is_int   ;
     json_number_to_int_fn    to_int   ;
     json_number_to_double_fn to_double;
     json_number_set_fn       set      ;
};

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

typedef enum {
     json_false=0,
     json_true,
     json_null,
} json_const_e;

typedef void         (*json_const_accept_fn) (json_const_t *this, json_visitor_t *visitor);
typedef void         (*json_const_free_fn  ) (json_const_t *this);
typedef json_const_e (*json_const_value_fn ) (json_const_t *this);

struct json_const {
     json_const_accept_fn accept;
     json_const_free_fn   free  ;
     json_const_value_fn  value ;
};

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

struct json_value {
     void (*accept) (json_value_t *this, json_visitor_t *visitor);
     void (*free)   (json_value_t *this);
};

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

struct json_visitor {
     void (*visit_object) (json_visitor_t *this, json_object_t *visited);
     void (*visit_array)  (json_visitor_t *this, json_array_t  *visited);
     void (*visit_string) (json_visitor_t *this, json_string_t *visited);
     void (*visit_number) (json_visitor_t *this, json_number_t *visited);
     void (*visit_const)  (json_visitor_t *this, json_const_t  *visited);
};

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

__PUBLIC__ json_object_t *json_new_object(json_memory_t memory);
__PUBLIC__ json_array_t  *json_new_array (json_memory_t memory);
__PUBLIC__ json_string_t *json_new_string(json_memory_t memory);
__PUBLIC__ json_number_t *json_new_number(json_memory_t memory);
__PUBLIC__ json_const_t  *json_const(json_const_e value);

#endif /* _YACJP_JSON_VALUE_H_ */
