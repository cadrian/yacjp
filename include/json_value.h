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
 * @file
 * The JSON values tree.
 *
 * All value pointers may be cast to/from `json_value_t*` and provide
 * the @ref json_value_accept_fn "accept()" and @ref
 * json_value_free_fn "free()" functions.
 */

#include <ctype.h>
#include <cad_shared.h>

/**
 * @addtogroup json_value
 * @{
 */

typedef struct json_value   json_value_t;

///@addtogroup json_object
typedef struct json_object  json_object_t;

///@addtogroup json_array
typedef struct json_array   json_array_t;

///@addtogroup json_string
typedef struct json_string  json_string_t;

///@addtogroup json_number
typedef struct json_number  json_number_t;

///@addtogroup json_const
typedef struct json_const   json_const_t;

typedef struct json_visitor json_visitor_t;


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/**
 * @addtogroup json_object
 * @{
 */

/**
 * Accepts a visitor.
 *
 * @param[in] this the target JSON object
 * @param[in] visitor the visitor
 */
typedef void                 (*json_object_accept_fn) (json_object_t *this, json_visitor_t *visitor);

/**
 * Frees the object and all its internal structures (but not its content).
 *
 * @param[in] this the target JSON object
 */
typedef void                 (*json_object_free_fn  ) (json_object_t *this);

/**
 * Counts the number of fields in this object.
 *
 * @param[in] this the target JSON object
 *
 * @return the number of fields in the object.
 */
typedef unsigned int         (*json_object_count_fn ) (json_object_t *this);

/**
 * Fills the provided keys array, which MUST be large enough (at least
 * @ref json_object_count_fn "count()" locations).
 *
 * @param[in] this the target JSON object
 */
typedef void                 (*json_object_keys_fn  ) (json_object_t *this, const char **keys);

/**
 * Gets a field.
 *
 * @param[in] this the target JSON object
 * @param[in] key the field name
 *
 * @return a field value, given its key.
 */
typedef json_value_t        *(*json_object_get_fn   ) (json_object_t *this, const char *key);

/**
 * Sets the provided new `value` to the given `key`.
 *
 * @param[in] this the target JSON object
 * @param[in] key the field name
 * @param[in] value the new field value
 *
 * @return the previous value, or `NULL` if none was set.
 */
typedef json_value_t        *(*json_object_set_fn   ) (json_object_t *this, const char *key, json_value_t *value);

/**
 * Removes both the provided `key` and its associated value from the
 * object.
 *
 * @param[in] this the target JSON object
 * @param[in] key the field name
 */
typedef json_value_t        *(*json_object_del_fn   ) (json_object_t *this, const char *key);

/**
 * The JSON object public interface.
 */
struct json_object {
     /**
      * @see json_object_accept_fn
      */
     json_object_accept_fn accept;
     /**
      * @see json_object_free_fn
      */
     json_object_free_fn   free  ;
     /**
      * @see json_object_count_fn
      */
     json_object_count_fn  count ;
     /**
      * @see json_object_keys_fn
      */
     json_object_keys_fn   keys  ;
     /**
      * @see json_object_get_fn
      */
     json_object_get_fn    get   ;
     /**
      * @see json_object_set_fn
      */
     json_object_set_fn    set   ;
     /**
      * @see json_object_del_fn
      */
     json_object_del_fn    del   ;
};

/**
 * @}
 */

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/**
 * @addtogroup json_array
 * @{
 */

/**
 * Accepts a visitor.
 *
 * @param[in] this the target JSON array
 * @param[in] visitor the visitor
 */
typedef void          (*json_array_accept_fn) (json_array_t *this, json_visitor_t *visitor);

/**
 * Frees the array and all its internal structures (but not its content).
 *
 * @param[in] this the target JSON array
 */
typedef void          (*json_array_free_fn  ) (json_array_t *this);

/**
 * Gets the number of values in this array
 *
 * @param[in] this the target JSON array
 *
 * @return the number of values in the array.
 */
typedef unsigned int  (*json_array_count_fn ) (json_array_t *this);

/**
 * Gets the `index`-th value in the array
 *
 * @param[in] this the target JSON array
 * @param[in] index the index of the value to get
 *
 * @return the value at the given index.
 */
typedef json_value_t *(*json_array_get_fn   ) (json_array_t *this, unsigned int index);

/**
 * Sets a `value` at the given `index`, maybe overwriting an existing
 * value. The array may be enlarged to accomodate the index.
 *
 * @param[in] this the target JSON array
 * @param[in] index the index of the value to put
 * @param[in] value the value to put
 */
typedef void          (*json_array_set_fn   ) (json_array_t *this, unsigned int index, json_value_t *value);

/**
 * Inserts a `value` at the given `index`, pushing the subsequent ones
 * if needed. The array may be enlarged to accomodate the index.
 *
 * @param[in] this the target JSON array
 * @param[in] index the index of the value to insert
 * @param[in] value the value to insert
 */
typedef void          (*json_array_ins_fn   ) (json_array_t *this, unsigned int index, json_value_t *value);

/**
 * Adds a value immediately after all the known ones (@ref
 * json_array_count_fn "count()" is incremented by exactly one).
 *
 * @param[in] this the target JSON array
 * @param[in] value the value to add
 */
typedef void          (*json_array_add_fn   ) (json_array_t *this,            json_value_t *value);

/**
 * Removes the value at the given `index` and pulls the subsequent
 * ones (@ref json_array_count_fn "count()" is decremented by exactly
 * one).
 *
 * Does nothing if the index is not between 0 included and
 * @ref json_array_count_fn "count()" excluded.
 *
 * @param[in] this the target JSON array
 * @param[in] index the index of the value to put
 */
typedef void          (*json_array_del_fn   ) (json_array_t *this, unsigned int index);

/**
 * The JSON array public interface.
 */
struct json_array {
     /**
      * @see json_array_accept_fn
      */
     json_array_accept_fn accept;
     /**
      * @see json_array_free_fn
      */
     json_array_free_fn   free  ;
     /**
      * @see json_array_count_fn
      */
     json_array_count_fn  count ;
     /**
      * @see json_array_get_fn
      */
     json_array_get_fn    get   ;
     /**
      * @see json_array_set_fn
      */
     json_array_set_fn    set   ;
     /**
      * @see json_array_set_fn
      */
     json_array_set_fn    ins   ;
     /**
      * @see json_array_add_fn
      */
     json_array_add_fn    add   ;
     /**
      * @see json_array_del_fn
      */
     json_array_del_fn    del   ;
};

/**
 * @}
 */

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/**
 * @addtogroup json_string
 * @{
 */

/**
 * A Unicode character (currently an unsigned int32)
 */
typedef __uint32_t unicode_char_t;

/**
 * Accepts a visitor.
 *
 * @param[in] this the target JSON string
 * @param[in] visitor the visitor
 */
typedef void            (*json_string_accept_fn   ) (json_string_t *this, json_visitor_t *visitor);

/**
 * Frees the string.
 *
 * @param[in] this the target JSON string
 */
typedef void            (*json_string_free_fn     ) (json_string_t *this);

/**
 * Counts the unicode characters.
 *
 * @param[in] this the target JSON string
 *
 * @return the number of unicode characters in the string
 */
typedef int             (*json_string_count_fn    ) (json_string_t *this);

/**
 * Puts the string encoded in utf-8 in the given `buffer`. The buffer
 * is guaranteed not to overflow the given `buffer_size`.
 *
 * @param[in] this the target JSON string
 * @param[in] buffer the buffer to fill
 * @param[in] buffer_size the size of the buffer
 *
 * @return the number of characters that are, or should have been,
 * written. The last '\\0' is written (or should be, if the buffer is
 * big enough) but NOT counted in the result. Thus if the result
 * greater than or equal to the given `buffer_size`, the user should call again
 * the function with an appropriately sized buffer.
 */
typedef size_t          (*json_string_utf8_fn     ) (json_string_t *this, char *buffer, size_t buffer_size);

/**
 * Gets the `index`-th unicode character in the string
 *
 * @param[in] this the target JSON string
 * @param[in] index the index of the unicode character to get
 *
 * @return the unicode character at the given index.
 */
typedef unicode_char_t  (*json_string_get_fn      ) (json_string_t *this, unsigned int index);

/**
 * Adds the given string (after eventual formatting) as a utf-8
 * encoded string.
 *
 * @param[in] this the target unicode string
 * @param[in] format the format of the bytes to put, compatible with printf()
 * @param[in] ... the arguments of the format
 */
typedef void            (*json_string_add_string_fn) (json_string_t *this, char *format, ...) __PRINTF__; /* expects a utf-8 encoded string */

/**
 * Tries to append the given utf-8 byte, maybe expecting more bytes to
 * finally add a full unicode character.
 *
 * @param[in] this the target JSON string
 * @param[in] c a utf-8 byte
 *
 * @return the number of characters still needed to add a valid
 * unicode character. If 0 then the character was sucessfully
 * added. If -1 there was an encoding error.
 */
typedef int             (*json_string_add_utf8_fn ) (json_string_t *this, char c);

/**
 * Adds the unicode character at the end of the string.
 *
 * @param[in] this the target JSON string
 * @param[in] unicode the unicode character to add
 */
typedef void            (*json_string_add_fn      ) (json_string_t *this, int unicode);

/**
 * The JSON unicode string public interface.
 */
struct json_string {
     /**
      * @see json_string_accept_fn
      */
     json_string_accept_fn     accept    ;
     /**
      * @see json_string_free_fn
      */
     json_string_free_fn       free      ;
     /**
      * @see json_string_count_fn
      */
     json_string_count_fn      count     ;
     /**
      * @see json_string_utf8_fn
      */
     json_string_utf8_fn       utf8      ;
     /**
      * @see json_string_get_fn
      */
     json_string_get_fn        get       ;
     /**
      * @see json_string_add_string_fn
      */
     json_string_add_string_fn add_string;
     /**
      * @see json_string_add_fn
      */
     json_string_add_fn        add       ;
     /**
      * @see json_string_add_utf8_fn
      */
     json_string_add_utf8_fn   add_utf8  ;
};

/**
 * @}
 */

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/**
 * @addtogroup json_number
 * @{
 */

/**
 * Accepts a visitor.
 *
 * @param[in] this the target JSON number
 * @param[in] visitor the visitor
 */
typedef void   (*json_number_accept_fn   ) (json_number_t *this, json_visitor_t *visitor);

/**
 * Frees the number.
 *
 * @param[in] this the target JSON number
 */
typedef void   (*json_number_free_fn     ) (json_number_t *this);

/**
 * Checks if the number can be represented as an int value
 *
 * @param[in] this the target JSON number
 *
 * @return 0 if the number cannot be represented as an int value, non-zero otherwise
 */
typedef int    (*json_number_is_int_fn   ) (json_number_t *this);

/**
 * Gets the number as an int.
 *
 * @pre `this->is_int(this)`
 *
 * @param[in] this the target JSON number
 *
 * @return the number as an int value, not specified if `!this->is_int(this)`
 *
 * @see json_number_is_int_fn
 */
typedef int    (*json_number_to_int_fn   ) (json_number_t *this);

/**
 * Gets the number as a double.
 *
 * @param[in] this the target JSON number
 *
 * @return the double value nearest to the number
 */
typedef double (*json_number_to_double_fn) (json_number_t *this);

/**
 * Sets the number.
 *
 * @param[in] this the target JSON number
 * @param[in] sign the sign of the number (1 or -1)
 * @param[in] integral the integral part
 * @param[in] decimal the decimal int part
 * @param[in] decimal_exp the decimal exponent; the actual decimal part is `decimal * 10^-decimal_exp`
 * @param[in] exp the exponent
 */
typedef void   (*json_number_set_fn      ) (json_number_t *this, int sign, unsigned int integral, unsigned int decimal, int decimal_exp, int exp);

/**
 * Fills the string buffer with the exact representation of the JSON
 * number. The buffer is guaranteed not to overflow the given size.
 *
 * @param[in] this the target JSON number
 * @param[in] buffer the buffer to fill
 * @param[in] buffer_size the size of `buffer`
 *
 * @return the number of characters that are, or should have been,
 * written. The last '\\0' is written (or should be, if the buffer is
 * big enough) but NOT counted in the result. Thus if the result
 * greater than or equal to the given buffer size, please call again
 * with a greater buffer.
 */
typedef int    (*json_number_to_string_fn) (json_number_t *this, char *buffer, size_t buffer_size);

/**
 * The JSON number public interface.
 */
struct json_number {
     /**
      * @see json_number_accept_fn
      */
     json_number_accept_fn    accept   ;
     /**
      * @see json_number_free_fn
      */
     json_number_free_fn      free     ;
     /**
      * @see json_number_is_int_fn
      */
     json_number_is_int_fn    is_int   ;
     /**
      * @see json_number_to_int_fn
      */
     json_number_to_int_fn    to_int   ;
     /**
      * @see json_number_to_double_fn
      */
     json_number_to_double_fn to_double;
     /**
      * @see json_number_set_fn
      */
     json_number_set_fn       set      ;
     /**
      * @see json_number_to_string_fn
      */
     json_number_to_string_fn to_string;
};

/**
 * @}
 */

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/**
 * @addtogroup json_const
 * @{
 */

/**
 * The known JSON constants
 */
typedef enum {
     /** true */
     json_false=0,
     /** false */
     json_true,
     /** null */
     json_null,
} json_const_e;

/**
 * Accepts a visitor.
 *
 * @param[in] this the target JSON const
 * @param[in] visitor the visitor
 */
typedef void         (*json_const_accept_fn) (json_const_t *this, json_visitor_t *visitor);

/**
 * Frees the object. As each const is actually a singleton that
 * function does nothing.
 *
 * @param[in] this the target JSON const
 */
typedef void         (*json_const_free_fn  ) (json_const_t *this);

/**
 * Gets the JSON const value.
 *
 * @param[in] this the target JSON const
 *
 * @return the JSON const value
 */
typedef json_const_e (*json_const_value_fn ) (json_const_t *this);

/**
 * The JSON constants public interface.
 */
struct json_const {
     /**
      * @see json_const_accept_fn
      */
     json_const_accept_fn accept;
     /**
      * @see json_const_free_fn
      */
     json_const_free_fn   free  ;
     /**
      * @see json_const_value_fn
      */
     json_const_value_fn  value ;
};

/**
 * @}
 */

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/**
 * Accepts a visitor.
 *
 * @param[in] this the target JSON value
 * @param[in] visitor the visitor
 */
typedef void (*json_value_accept_fn) (json_value_t *this, json_visitor_t *visitor);

/**
 * Frees the JSON value.
 *
 * @param[in] this the target JSON value
 */
typedef void (*json_value_free_fn  ) (json_value_t *this);

/**
 * The JSON values public interface. This interface is common to @ref
 * json_object, @ref json_array, @ref json_string, @ref json_number,
 * and @ref json_const.
 */
struct json_value {
     /**
      * @see json_value_accept_fn
      */
     json_value_accept_fn accept;
     /**
      * @see json_value_free_fn
      */
     json_value_free_fn   free  ;
};

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/**
 * Frees the visitor.
 *
 * @param[in] this the target visitor
 */
typedef void (*json_visit_free_fn  ) (json_visitor_t *this);

/**
 * Visits an object.
 *
 * @param[in] this the target visitor
 * @param[in] visited the visited object
 */
typedef void (*json_visit_object_fn) (json_visitor_t *this, json_object_t *visited);

/**
 * Visits an array.
 *
 * @param[in] this the target visitor
 * @param[in] visited the visited array
 */
typedef void (*json_visit_array_fn ) (json_visitor_t *this, json_array_t  *visited);

/**
 * Visits a string.
 *
 * @param[in] this the target visitor
 * @param[in] visited the visited string
 */
typedef void (*json_visit_string_fn) (json_visitor_t *this, json_string_t *visited);

/**
 * Visits a number.
 *
 * @param[in] this the target visitor
 * @param[in] visited the visited number
 */
typedef void (*json_visit_number_fn) (json_visitor_t *this, json_number_t *visited);

/**
 * Visits a const.
 *
 * @param[in] this the target visitor
 * @param[in] visited the visited const
 */
typedef void (*json_visit_const_fn ) (json_visitor_t *this, json_const_t  *visited);

/**
 * The \em Visitor Design Pattern in action: the structure has a
 * function pointer for each type of @ref json_value.
 */
struct json_visitor {
     /**
      * @see json_visit_free_fn
      */
     json_visit_free_fn   free;
     /**
      * @see json_visit_object_fn
      */
     json_visit_object_fn visit_object;
     /**
      * @see json_visit_array_fn
      */
     json_visit_array_fn  visit_array ;
     /**
      * @see json_visit_string_fn
      */
     json_visit_string_fn visit_string;
     /**
      * @see json_visit_number_fn
      */
     json_visit_number_fn visit_number;
     /**
      * @see json_visit_const_fn
      */
     json_visit_const_fn  visit_const ;
};

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/**
 * Creates a new object and returns it.
 *
 * @param[in] memory the memory manager
 *
 * @return the new object
 */
__PUBLIC__ json_object_t *json_new_object(cad_memory_t memory);

/**
 * Creates a new array and returns it.
 *
 * @param[in] memory the memory manager
 *
 * @return the new array
 */
__PUBLIC__ json_array_t  *json_new_array (cad_memory_t memory);

/**
 * Creates a new string and returns it.
 *
 * @param[in] memory the memory manager
 *
 * @return the new string
 */
__PUBLIC__ json_string_t *json_new_string(cad_memory_t memory);

/**
 * Creates a new number and returns it.
 *
 * @param[in] memory the memory manager
 *
 * @return the new number
 */
__PUBLIC__ json_number_t *json_new_number(cad_memory_t memory);

/**
 * Gets the JSON const value associated with the given const.
 *
 * @param[in] value the value to return
 *
 * @return the unique (shared) value associated with the given const
 */
__PUBLIC__ json_const_t  *json_const(json_const_e value);

/**
 * @}
 */

#endif /* _YACJP_JSON_VALUE_H_ */
