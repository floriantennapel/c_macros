#ifndef QUEUE_H
#define QUEUE_H

#include <stddef.h>
#include <assert.h>
#include <stdlib.h>

/*****************************************************************************
* Generates functions for a new Queue datastructure
*
* @param QUEUE_NAME name of owner struct and prefix of each function defined
* @param QUEUE_VAL_TYPE type stored in queue
******************************************************************************/
#define QUEUE_DEFINE(QUEUE_NAME, QUEUE_VAL_TYPE) \
    struct _##QUEUE_NAME##_NODE \
    { \
        QUEUE_VAL_TYPE value; \
        struct _##QUEUE_NAME##_NODE* next; \
    } _##QUEUE_NAME##_NODE; \
    \
    typedef struct \
    { \
        struct _##QUEUE_NAME##_NODE* head; \
        struct _##QUEUE_NAME##_NODE* tail; \
        size_t size; \
    } QUEUE_NAME; \
    \
    \
    /***************************
    * Makes a new empty queue
    ****************************/ \
    QUEUE_NAME QUEUE_NAME##_new() \
    { \
        return (QUEUE_NAME) {NULL, NULL, 0}; \
    } \
    \
    \
    /***********************************************************************************
    * Pushes a value to the back of the queue, the value is copied and stored in place
    ************************************************************************************/ \
    void QUEUE_NAME##_push(QUEUE_NAME* q, QUEUE_VAL_TYPE value) \
    { \
        assert(q); \
        struct _##QUEUE_NAME##_NODE* new_ptr = malloc(sizeof(_##QUEUE_NAME##_NODE)); \
        assert(new_ptr); \
        *new_ptr = (struct _##QUEUE_NAME##_NODE) {value, NULL}; \
        if (!q->tail) \
            q->head = new_ptr; \
        else \
            q->tail->next = new_ptr; \
        q->tail = new_ptr; \
        ++(q->size); \
    } \
    \
    \
    /*********************************************************************************************
    * Deallocates and removes the front of the queue and returns the value that was stored there
    **********************************************************************************************/ \
    QUEUE_VAL_TYPE QUEUE_NAME##_pop(QUEUE_NAME* q) \
    { \
        assert(q); \
        assert((q->size)--); \
        struct _##QUEUE_NAME##_NODE* tmp = q->head; \
        q->head = tmp->next; \
        QUEUE_VAL_TYPE ret = tmp->value; \
        assert(tmp); \
        free(tmp); \
        return ret; \
    }

#endif
