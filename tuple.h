#ifndef TUPLE_H
#define TUPLE_H

/**************************
 * A generic hashable tuple
 *
 * Requires C20 or newer
 **************************/

#include <string.h>
#include "datastructures/hashmap.h"

// Recursive macros with C++20 __VA_OPT__
// by David Mazières
// https://www.scs.stanford.edu/~dm/blog/va-opt.html 
#define _TUPLE_PARENS ()

//re-evaluates TUPLE_FOR_EACH_HELPER a maximum of 256 (4^4) times
#define _TUPLE_EXPAND(...) _TUPLE_EXPAND4(_TUPLE_EXPAND4(_TUPLE_EXPAND4(_TUPLE_EXPAND4(__VA_ARGS__))))
#define _TUPLE_EXPAND4(...) _TUPLE_EXPAND3(_TUPLE_EXPAND3(_TUPLE_EXPAND3(_TUPLE_EXPAND3(__VA_ARGS__))))
#define _TUPLE_EXPAND3(...) _TUPLE_EXPAND2(_TUPLE_EXPAND2(_TUPLE_EXPAND2(_TUPLE_EXPAND2(__VA_ARGS__))))
#define _TUPLE_EXPAND2(...) _TUPLE_EXPAND1(_TUPLE_EXPAND1(_TUPLE_EXPAND1(_TUPLE_EXPAND1(__VA_ARGS__))))
#define _TUPLE_EXPAND1(...) __VA_ARGS__

#define _TUPLE_FOR_EACH(macro, ...)                                    \
  __VA_OPT__(_TUPLE_EXPAND(_TUPLE_FOR_EACH_HELPER(macro, __VA_ARGS__)))

//recursively calls itself until __VA_ARGS__ is empty
#define _TUPLE_FOR_EACH_HELPER(macro, a1, ...)                         \
  macro(a1)                                                     \
  __VA_OPT__(_TUPLE_FOR_EACH_AGAIN _TUPLE_PARENS (macro, __VA_ARGS__))
#define _TUPLE_FOR_EACH_AGAIN() _TUPLE_FOR_EACH_HELPER

#define _TUPLE_ADD_SEMICOLON(type_and_name) type_and_name;

/*******************************************************************
* Generates code for a hashable tuple
*
* equality and hash is done on values directly stored, 
* so if fields contain pointers the behaviour might be unexpected
*
* @param TUPLE_NAME name of struct and prefix of related functions
* @param fields valid field declarations, only one per line, 
*   currently up to 256 field declarations are supported
*
* EXAMPLE USAGE:
*
* //defining a new Triple type
* TUPLE_DEFINE(Triple, int a, int b, char c[20])
*
* // It can now be initialized as any other struct
* Triple t = {4, 2, "hi"};
*
* // it also has a hash function implemented
* size_t hashcode = Triple_hasher(&t);
********************************************************************/
#define TUPLE_DEFINE(TUPLE_NAME, fields...) \
    typedef struct \
    { \
        _TUPLE_FOR_EACH(_TUPLE_ADD_SEMICOLON, fields) \
    } TUPLE_NAME; \
    \
    \
    /***********************************************************
     * Hashes Tuple, 
     * the byte representation of the tuple is hashed, 
     * i.e. all fields as they are stored combined in one hash 
     ***********************************************************/ \
    size_t TUPLE_NAME##_hash(const TUPLE_NAME* p) \
    { \
        return byte_hasher((const char*)(p), sizeof(TUPLE_NAME)); \
    } \
    \
    \
    /*************************************************************
     * Checks if the byte representation of the tuples are equal
     * i.e. if the fields contain the same values
     *************************************************************/ \
    bool TUPLE_NAME##_eq(const TUPLE_NAME* a, const TUPLE_NAME* b) \
    { \
        return memcmp(a, b, sizeof(TUPLE_NAME)) == 0; \
    }

#endif
