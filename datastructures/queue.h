#ifndef QUEUE_H
#define QUEUE_H

#include <stddef.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

/*****************************************************************************
* Generates functions for a new Queue datastructure
*
* @param QUEUE_NAME name of owner struct and prefix of each function defined
* @param QUEUE_VAL_TYPE type stored in queue
******************************************************************************/
#define QUEUE_DEFINE(QUEUE_NAME, QUEUE_VAL_TYPE) \
    typedef struct \
    { \
        size_t size, _capacity, _head, _tail; \
        QUEUE_VAL_TYPE* _arr; \
    } QUEUE_NAME; \
    \
    \
    /***********************************************************************
    * Makes a new empty queue
    *
    * @param initial_capacity should be set to expected number of entries, 
    *   but any value is fine, as it will resize as needed
    ************************************************************************/ \
    static QUEUE_NAME QUEUE_NAME##_new(size_t initial_capacity) \
    { \
        size_t capacity = 1; \
        for (; capacity <= initial_capacity; capacity <<= 1); \
        QUEUE_NAME ret = {0, capacity, 0, 0, NULL}; \
        ret._arr = malloc(sizeof(QUEUE_VAL_TYPE) * capacity); \
        assert(ret._arr); \
        return ret; \
    } \
    \
    \
    /**************************************
     * Do not use this function
     *
     * Resizes underlying array if needed 
     **************************************/ \
    static void _##QUEUE_NAME##_resize(QUEUE_NAME* q, size_t new_capacity) \
    { \
        QUEUE_VAL_TYPE* old_arr = q->_arr; \
        size_t old_cap = q->_capacity; \
        q->_capacity = new_capacity; \
        q->_arr = malloc(sizeof(QUEUE_VAL_TYPE) * q->_capacity); \
        assert(q->_arr); \
        \
        QUEUE_VAL_TYPE* head_ptr = old_arr+q->_head; \
        if (q->_tail <= q->_head) { \
            size_t back_bytes = old_cap - q->_head; \
            memcpy(q->_arr, head_ptr, sizeof(QUEUE_VAL_TYPE) * back_bytes); \
            memcpy(q->_arr + back_bytes, old_arr, sizeof(QUEUE_VAL_TYPE) * (q->size - back_bytes)); \
        } else { \
            memcpy(q->_arr, head_ptr, sizeof(QUEUE_VAL_TYPE) * q->size); \
        } \
        q->_head = 0; \
        q->_tail = q->size; \
        \
        free(old_arr); \
    } \
    \
    \
    /***********************************************************************************
    * Pushes a value to the back of the queue, the value is copied and stored in place
    ************************************************************************************/ \
    static void QUEUE_NAME##_push(QUEUE_NAME* q, QUEUE_VAL_TYPE value) \
    { \
        assert(q); \
        if (q->size == q->_capacity) \
            _##QUEUE_NAME##_resize(q, q->_capacity * 2); \
        (q->size)++; \
        q->_arr[q->_tail] = value; \
        q->_tail = (q->_tail + 1) % q->_capacity; \
    } \
    \
    \
    /*********************************************************************************************
    * Deallocates and removes the front of the queue and returns the value that was stored there
    **********************************************************************************************/ \
    static QUEUE_VAL_TYPE QUEUE_NAME##_pop(QUEUE_NAME* q) \
    { \
        assert(q); \
        assert((q->size)--); \
        QUEUE_VAL_TYPE ret = q->_arr[q->_head]; \
        q->_head = (q->_head + 1) % q->_capacity; \
        if (q->size < q->_capacity / 4. && q->size > 16) \
            _##QUEUE_NAME##_resize(q, q->_capacity / 2); \
        return ret; \
    }

#endif
