#ifndef HEAP_H
#define HEAP_H

#include <assert.h>
#include <stddef.h>

#include "vec.h"

#define _HEAP_LEFT(i) ((i) * 2 + 1)
#define _HEAP_RIGHT(i) ((i) * 2 + 2)
#define _HEAP_PARENT(i) (((i)-1) / 2)

/***************************************************************************************
* Creates functions for a min-heap, dependent on a base Vector type
*
* @param HEAP_NAME prefix of function names
* @param HEAP_VAL_TYPE type of values stored in heap
* @param HEAP_VAL_CMP_FUNC int (*)(const <HEAP_VAL_TYPE>* a, const <HEAP_VAL_TYPE>* b)
*   function or macro to compare two entries. 
*   Given two entries a and b, should return less than 0 if a < b, 
*   more than 0 if a > b and 0 if a == b
*   If a max heap is desired, this function should return the opposite ordering
****************************************************************************************/
#define HEAP_DEFINE(HEAP_NAME, HEAP_VAL_TYPE, HEAP_VAL_CMP_FUNC) \
    VEC_DEFINE(_##HEAP_NAME##_VECTOR_TYPE, HEAP_VAL_TYPE) \
    typedef _##HEAP_NAME##_VECTOR_TYPE HEAP_NAME; \
    \
    /**************************
     * Returns a new min-heap
     **************************/ \
    HEAP_NAME HEAP_NAME##_new() \
    { \
        return _##HEAP_NAME##_VECTOR_TYPE##_new(0); \
    } \
    /*******************************************
     * Sifts the element at a given index down 
     * until the heap invariant is restored
     *******************************************/ \
    void _##HEAP_NAME##_sift_down(HEAP_NAME* vector, size_t i) \
    { \
        size_t left = _HEAP_LEFT(i), right = _HEAP_RIGHT(i), min_child_ind; \
        while (left < vector->size) { \
            if (right < vector->size) { \
                int _cmp_res = HEAP_VAL_CMP_FUNC( \
                    ((const HEAP_VAL_TYPE*)(vector->arr+left)), \
                    ((const HEAP_VAL_TYPE*)(vector->arr+right)) \
                ); \
                min_child_ind = _cmp_res <= 0 ? left : right; \
            } else \
                min_child_ind = left; \
            \
            if ((HEAP_VAL_CMP_FUNC(((const HEAP_VAL_TYPE*)(vector->arr+i)), ((const HEAP_VAL_TYPE*)(vector->arr+min_child_ind)))) <= 0) \
                break; \
            HEAP_VAL_TYPE tmp = vector->arr[i]; \
            vector->arr[i] = vector->arr[min_child_ind]; \
            vector->arr[min_child_ind] = tmp; \
            i = min_child_ind; \
            left = _HEAP_LEFT(i); \
            right = _HEAP_RIGHT(i); \
        } \
    \
    } \
    \
    \
    /**********************************************
    * Constructs a min-heap from the given vector
    *
    * @param vector Can have any vector type with 
    *   identical <VALUE_TYPE> as the heap
    ***********************************************/ \
    void HEAP_NAME##_heapify(void* vector) \
    { \
        _##HEAP_NAME##_VECTOR_TYPE* cast_vec = (_##HEAP_NAME##_VECTOR_TYPE*) vector; \
        size_t i = _HEAP_PARENT((cast_vec->size - 1)); \
        do { \
            _##HEAP_NAME##_sift_down(cast_vec, i); \
        } while (i--); \
    } \
    \
    \
    /************************************
     * Inserts an element into the Heap
     ************************************/ \
    void HEAP_NAME##_push(HEAP_NAME* vector, HEAP_VAL_TYPE value) \
    { \
        assert(vector); \
        _##HEAP_NAME##_VECTOR_TYPE##_push(vector, value); \
        size_t ind = vector->size-1; \
        size_t par_ind = _HEAP_PARENT(ind); \
        while (ind > 0 && (HEAP_VAL_CMP_FUNC(((const HEAP_VAL_TYPE*)(vector->arr+ind)), ((const HEAP_VAL_TYPE*)(vector->arr+par_ind)))) < 0) { \
            HEAP_VAL_TYPE tmp = vector->arr[ind]; \
            vector->arr[ind] = vector->arr[par_ind]; \
            vector->arr[par_ind] = tmp; \
            ind = par_ind; \
            par_ind = _HEAP_PARENT(ind); \
        } \
    } \
    \
    \
    /************************************************************
     * Removes the minimum element from the Heap and returns it
     ************************************************************/ \
    HEAP_VAL_TYPE HEAP_NAME##_pop(HEAP_NAME* vector) \
    { \
        assert(vector); \
        HEAP_VAL_TYPE ret = vector->arr[0]; \
        vector->arr[0] = _##HEAP_NAME##_VECTOR_TYPE##_pop(vector); \
        _##HEAP_NAME##_sift_down(vector, 0); \
        return ret; \
    } \
    \
    /**************************************
     * deallocates resources used by heap
     **************************************/ \
    void HEAP_NAME##_free(HEAP_NAME* heap) \
    { \
        _##HEAP_NAME##_VECTOR_TYPE_free(heap); \
    }

#endif
