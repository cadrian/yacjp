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

/**
 * The JSON values tree.
 *
 * All value pointers may be cast to/from json_value_t* and provided
 * the accept() and free() functions.
 */

#include <ctype.h>

#include "json_shared.h"

typedef struct json_value   json_value_t;

typedef struct json_object  json_object_t;
typedef struct json_array   json_array_t;
typedef struct json_string  json_string_t;
typedef struct json_number  json_number_t;
typedef struct json_const   json_const_t;

typedef struct json_visitor json_visitor_t;


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

typedef void                 (*json_object_accept_fn) (json_object_t *this, json_visitor_t *visitor);
typedef void                 (*json_object_free_fn  ) (json_object_t *this);
typedef unsigned int         (*json_object_count_fn ) (json_object_t *this);
typedef void                 (*json_object_keys_fn  ) (json_object_t *this, const char **keys);
typedef json_value_t        *(*json_object_get_fn   ) (json_object_t *this, const char *key);
typedef json_value_t        *(*json_object_set_fn   ) (json_object_t *this, const char *key, json_value_t *value);
typedef json_value_t        *(*json_object_del_fn   ) (json_object_t *this, const char *key);

/**
 * The JSON object public interface.
 */
struct json_object {
     /**
      * Accept a visitor.
      */
     json_object_accept_fn accept;

     /**
      * Free the object and all its internal structures (but not its content).
      */
     json_object_free_fn   free  ;

     /**
      * @return the number of values in the object.
      */
     json_object_count_fn  count ;

     /**
      * Fill the provided keys array, which MUST be large enough (at
      * least count() locations)
      */
     json_object_keys_fn   keys  ;

     /**
      * @return a value, given its key.
      */
     json_object_get_fn    get   ;

     /**
      * Set the provided new value to the given key.
      *
      * @return the previous value, or NULL if none was set.
      */
     json_object_set_fn    set   ;

     /**
      * Remove both the provided key and its associated value from the
      * object.
      */
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

/**
 * The JSON array public interface.
 */
struct json_array {
     /**
      * Accept a visitor.
      */
     json_array_accept_fn accept;

     /**
      * Free the array and all its internal structures (but not its content).
      */
     json_array_free_fn   free  ;

     /**
      * @return the number of values in the array.
      */
     json_array_count_fn  count ;

     /**
      * @return the value at the given index.
      */
     json_array_get_fn    get   ;

     /**
      * Set a value at the given index, maybe overwriting an existing
      * value. The array may be enlarged to accomodate the index.
      */
     json_array_set_fn    set   ;

     /**
      * Insert a value at the given index, pushing the subsequent ones
      * if needed. The array may be enlarged to accomodate the index.
      */
     json_array_set_fn    ins   ;

     /**
      * Add a value immediately after all the known ones (count() is
      * incremented by exactly one).
      */
     json_array_add_fn    add   ;

     /**
      * Removes the value at the given index and pulls the subsequent
      * ones (count() is decremented by exactly one).
      *
      * Does nothing if the index is not between 0 included and
      * count() excluded.
      */
     json_array_del_fn    del   ;
};

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


typedef __uint32_t unicode_char_t;

typedef void            (*json_string_accept_fn  ) (json_string_t *this, json_visitor_t *visitor);
typedef void            (*json_string_free_fn    ) (json_string_t *this);
typedef int             (*json_string_count_fn   ) (json_string_t *this);
typedef size_t          (*json_string_utf8_fn    ) (json_string_t *this, char *buffer, size_t buffer_size);
typedef unicode_char_t  (*json_string_get_fn     ) (json_string_t *this, unsigned int index);
typedef void            (*json_string_set_fn     ) (json_string_t *this, char *format, ...) __attribute__((format(printf, 2, 3))); /* expects a utf-8 encoded string */
typedef int             (*json_string_add_utf8_fn) (json_string_t *this, char c); /* returns the number of expected extra chars to read to form a correct unicode character; -1 if error */
typedef void            (*json_string_add_fn     ) (json_string_t *this, int unicode);

/**
 * The JSON unicode string public interface.
 */
struct json_string {
     /**
      * Accept a visitor.
      */
     json_string_accept_fn   accept  ;

     /**
      * Free the string.
      */
     json_string_free_fn     free    ;

     /**
      * Count the unicode characters.
      */
     json_string_count_fn    count   ;

     /**
      * Puts the string encoded in utf-8 in the given buffer. The
      * buffer is guaranteed not to overflow the given size.
      *
      * @return the number of characters that are, or should have
      * been, written. The last '\0' is written (or should be, if the
      * buffer is not big enough) but NOT counted in the result. Thus
      * if the result greater than or equal to the given buffer size,
      * please call again with a greater buffer.
      */
     json_string_utf8_fn     utf8    ;

     /**
      * @return the unicode character at the given index.
      */
     json_string_get_fn      get     ;

     /**
      * ADD the given string (after eventual formatting) as a utf-8
      * encoded string.
      */
     json_string_set_fn      set     ;

     /**
      * Add the unicode character at the end of the string.
      */
     json_string_add_fn      add     ;

     /**
      * Tries to append the given utf-8 character, maybe expecting
      * more to finally add a full unicode character.
      *
      * @return the number of characters still needed to add a valid
      * unicode character. If 0 then the character was sucessfully
      * added.
      */
     json_string_add_utf8_fn add_utf8;
};

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

typedef void   (*json_number_accept_fn   ) (json_number_t *this, json_visitor_t *visitor);
typedef void   (*json_number_free_fn     ) (json_number_t *this);
typedef int    (*json_number_is_int_fn   ) (json_number_t *this);
typedef int    (*json_number_to_int_fn   ) (json_number_t *this);
typedef double (*json_number_to_double_fn) (json_number_t *this);
typedef void   (*json_number_set_fn      ) (json_number_t *this, int integral, int decimal, int decimal_exp, int exp);
typedef int    (*json_number_to_string_fn) (json_number_t *this, char *buffer, size_t buffer_size);

struct json_number {
     /**
      * Accept a visitor.
      */
     json_number_accept_fn    accept   ;
     json_number_free_fn      free     ;
     json_number_is_int_fn    is_int   ;
     json_number_to_int_fn    to_int   ;
     json_number_to_double_fn to_double;
     json_number_set_fn       set      ;
     json_number_to_string_fn to_string;
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
     /**
      * Accept a visitor.
      */
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

typedef void (*json_visit_del_fn   ) (json_visitor_t *this);
typedef void (*json_visit_object_fn) (json_visitor_t *this, json_object_t *visited);
typedef void (*json_visit_array_fn ) (json_visitor_t *this, json_array_t  *visited);
typedef void (*json_visit_string_fn) (json_visitor_t *this, json_string_t *visited);
typedef void (*json_visit_number_fn) (json_visitor_t *this, json_number_t *visited);
typedef void (*json_visit_const_fn ) (json_visitor_t *this, json_const_t  *visited);

struct json_visitor {
     json_visit_del_fn    del;
     json_visit_object_fn visit_object;
     json_visit_array_fn  visit_array ;
     json_visit_string_fn visit_string;
     json_visit_number_fn visit_number;
     json_visit_const_fn  visit_const ;
};

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

__PUBLIC__ json_object_t *json_new_object(json_memory_t memory);
__PUBLIC__ json_array_t  *json_new_array (json_memory_t memory);
__PUBLIC__ json_string_t *json_new_string(json_memory_t memory);
__PUBLIC__ json_number_t *json_new_number(json_memory_t memory);
__PUBLIC__ json_const_t  *json_const(json_const_e value);

#endif /* _YACJP_JSON_VALUE_H_ */
