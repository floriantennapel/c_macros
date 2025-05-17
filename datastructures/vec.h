#ifndef VEC_H
#define VEC_H

#include <stdlib.h>
#include <assert.h>
#include <stddef.h>

/*****************************************************************************
* Generates functions for a new Vec datastructure
*
* Functions to access and set elements of the are not defined, 
* this should be done using the underlying _arr field
*
* Functions with linear runtime, like insertion and removal are also not defined.
* Use another datastructure, or add the methods later if this is needed.
*
* @param VEC_NAME name of owner struct and prefix of each function defined
* @param VEC_VAL_TYPE type stored in the Vec
******************************************************************************/
#define VEC_DEFINE(VEC_NAME, VEC_VAL_TYPE) \
    typedef struct \
    { \
        VEC_VAL_TYPE* _arr; \
        size_t _arr_cap; \
        size_t size; \
    } VEC_NAME; \
    \
    \
    /**********************************************
    * Makes a new Vec filled with zeroes
    *
    * @param initial_size initial size of the Vec
    ***********************************************/ \
    VEC_NAME VEC_NAME##_new(size_t initial_size) \
    { \
        assert(initial_size >= 0); \
        size_t initial_capacity = 1; \
        for (; initial_capacity < initial_size; initial_capacity <<= 1); \
        VEC_VAL_TYPE* mem = calloc(initial_capacity, sizeof(VEC_VAL_TYPE)); \
        assert(mem); \
        return (VEC_NAME) {mem, initial_capacity, initial_size}; \
    } \
    \
    \
    /*********************************************************************************
    * Pushes a value to the back of the Vec, the value is copied and stored in place
    *
    * If the underlying array needs resizing, it is doubled in size using realloc.
    **********************************************************************************/ \
    void VEC_NAME##_push(VEC_NAME* vec, VEC_VAL_TYPE value) \
    { \
        assert(vec); \
        if (vec->size == vec->_arr_cap) { \
            vec->_arr_cap *= 2; \
            vec->_arr = realloc(vec->_arr, vec->_arr_cap * sizeof(VEC_VAL_TYPE)); \
            assert(vec->_arr); \
        } \
        vec->_arr[(vec->size)++] = value; \
    } \
    \
    \
    /**************************************************************************************************************
    * Removes the last element of the Vec and returns the value that was stored there
    *
    * The underlying array is resized using realloc if it is more than four times the size of the stored elements
    ***************************************************************************************************************/ \
    VEC_VAL_TYPE VEC_NAME##_pop(VEC_NAME* vec) \
    { \
        assert(vec); \
        assert(vec->size); \
        VEC_VAL_TYPE ret = vec->_arr[--(vec->size)]; \
        if (vec->size < vec->_arr_cap / 4) { \
            vec->_arr_cap /= 2; \
            vec->_arr = realloc(vec->_arr, vec->_arr_cap * sizeof(VEC_VAL_TYPE)); \
            assert(vec->_arr); \
        } \
        return ret; \
    } \

#endif
