#ifndef VEC_H
#define VEC_H

#include <stdlib.h>
#include <assert.h>
#include <stddef.h>
#include <string.h>

/*****************************************************************************
* Generates functions for a new Vec datastructure
*
* Functions to access and set elements of the are not defined, 
* this should be done using the underlying arr field
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
        VEC_VAL_TYPE* arr; \
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
    /**************************************************
     * Creates a new Vector that is a copy of another
     *
     * @param copy_from valid initialized vector
     **************************************************/ \
    VEC_NAME VEC_NAME##_copy(const VEC_NAME* copy_from) \
    { \
        assert(copy_from); \
        VEC_VAL_TYPE* mem = calloc(copy_from->_arr_cap, sizeof(VEC_VAL_TYPE)); \
        memcpy(mem, copy_from->arr, copy_from->size * sizeof(VEC_VAL_TYPE)); \
        assert(mem); \
        return (VEC_NAME) {mem, copy_from->_arr_cap, copy_from->size}; \
    } \
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
            vec->arr = realloc(vec->arr, vec->_arr_cap * sizeof(VEC_VAL_TYPE)); \
            assert(vec->arr); \
        } \
        vec->arr[(vec->size)++] = value; \
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
        VEC_VAL_TYPE ret = vec->arr[--(vec->size)]; \
        if (vec->size < vec->_arr_cap / 4) { \
            vec->_arr_cap /= 2; \
            vec->arr = realloc(vec->arr, vec->_arr_cap * sizeof(VEC_VAL_TYPE)); \
            assert(vec->arr); \
        } \
        return ret; \
    } \
    \
    \
    /************************************
    * Deallocates memory used by vector
    *
    * Do not use after this point
    *************************************/ \
    void VEC_NAME##_free(VEC_NAME* vec) \
    { \
        assert(vec); \
        free(vec->arr); \
    } \
    \
    \
    /*********************************
    * Removes all elements in vector
    *
    * Safe to use after this
    **********************************/ \
    void VEC_NAME##_clear(VEC_NAME* vec) \
    { \
        assert(vec); \
        vec->size = 0; \
        vec->_arr_cap = 1; \
        vec->arr = realloc(vec->arr, sizeof(VEC_VAL_TYPE)); \
        assert(vec->arr); \
    }

#endif
