#ifndef TREEMAP_H
#define TREEMAP_H

#include <stddef.h>
#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>

typedef struct
{} TREEMAP_NO_VALUE;

// Code taken and modified from the paper: "Left-leaning Red-Black Trees" 
// by Robert Sedgewick, 2008

#define _TREEMAP_ROTATE_LEFT(OUT, NODE_TYPE, NODE) \
{ \
    NODE_TYPE* X = (NODE)->_right; \
    (NODE)->_right = X->_left; \
    X->_left = (NODE); \
    X->_color = (NODE)->_color; \
    (NODE)->_color = 1; \
    (OUT) = X; \
} 

#define _TREEMAP_ROTATE_RIGHT(OUT, NODE_TYPE, NODE) \
{ \
    NODE_TYPE* X = (NODE)->_left; \
    (NODE)->_left = X->_right; \
    X->_right = (NODE); \
    X->_color = (NODE)->_color; \
    (NODE)->_color = 1; \
    (OUT) = X; \
} 

#define _TREEMAP_IS_RED(NODE) ((NODE) && (NODE)->_color)

#define TREEMAP_DEFINE(TREEMAP_NAME, TREEMAP_KEY_TYPE, TREEMAP_VAL_TYPE, TREEMAP_KEY_CMP) \
    typedef struct TREEMAP_NAME##Node \
    { \
        TREEMAP_KEY_TYPE key; \
        TREEMAP_VAL_TYPE value; \
        struct TREEMAP_NAME##Node *_left, *_right; \
        unsigned _color: 1; \
    } TREEMAP_NAME##Node; \
    \
    typedef struct \
    { \
        TREEMAP_NAME##Node* root; \
        size_t size; \
    } TREEMAP_NAME; \
    \
    \
    typedef struct \
    { \
        TREEMAP_NAME##Node* current; \
        unsigned _stack_size; \
        TREEMAP_NAME##Node* _callstack[200]; \
    } TREEMAP_NAME##Iter; \
    \
    \
    TREEMAP_NAME TREEMAP_NAME##_new() \
    { \
        return (TREEMAP_NAME) {NULL, 0}; \
    } \
    \
    \
    /***************************
    * Do not use this function
    ****************************/ \
    TREEMAP_NAME##Node* _##TREEMAP_NAME##_search_helper(TREEMAP_NAME* tree, TREEMAP_NAME##Node* node, const TREEMAP_KEY_TYPE* key, bool insert, TREEMAP_NAME##Node** search_result) \
    { \
        if (!node) { \
            if (insert) { \
                TREEMAP_NAME##Node* ret = calloc(1, sizeof(TREEMAP_NAME##Node)); \
                assert(ret != NULL); \
                (tree->size)++; \
                ret->key = *key; \
                ret->_color = 1; \
                *search_result = ret; \
                return ret; \
            } else { \
                *search_result = NULL; \
                return NULL; \
            } \
        } \
        \
        if (insert && _TREEMAP_IS_RED(node->_left) && _TREEMAP_IS_RED(node->_right)) { \
            node->_left->_color = 0; \
            node->_right->_color = 0; \
            node->_color = !(node->_color); \
        } \
        \
        int cmp_res = TREEMAP_KEY_CMP(key, &(node->key)); \
        if (!cmp_res) \
            *search_result = node; \
        else if (cmp_res < 0) \
            node->_left = _##TREEMAP_NAME##_search_helper(tree, node->_left, key, insert, search_result); \
        else \
            node->_right = _##TREEMAP_NAME##_search_helper(tree, node->_right, key, insert, search_result); \
        \
        if (insert && _TREEMAP_IS_RED(node->_right) && !_TREEMAP_IS_RED(node->_left)) \
            _TREEMAP_ROTATE_LEFT(node, TREEMAP_NAME##Node, node); \
        if (insert && _TREEMAP_IS_RED(node->_left) && _TREEMAP_IS_RED(node->_left->_left)) \
            _TREEMAP_ROTATE_RIGHT(node, TREEMAP_NAME##Node, node); \
        \
        return node; \
    } \
    \
    TREEMAP_NAME##Node* TREEMAP_NAME##_search(TREEMAP_NAME* map, const TREEMAP_KEY_TYPE* key, bool insert) \
    { \
        assert(map != NULL); \
        assert(key != NULL); \
        TREEMAP_NAME##Node* res; \
        map->root = _##TREEMAP_NAME##_search_helper(map, map->root, key, insert, &res); \
        if (map->root) \
            map->root->_color = 0; \
        return res; \
    } \
    \
    \
    /* todo deletion */ \
    /**************************************************************************************
     * creates an Iterator to iterate over all elements efficiently
     * the field current holds the current element, or NULL if there are no more elements
     * Iter_inc moves the iterator one step forwards
     * 
     * If the key is not found, the iterator will be empty, with no elements
     *
     * @param key starts iterator at the element matching this key. 
     *    If NULL is passed, the iterator starts at the first element of the tree
     *
     * Does not own any memory, so no deallocation is needed afterwards
     **************************************************************************************/ \
    TREEMAP_NAME##Iter TREEMAP_NAME##_get_iter(const TREEMAP_NAME* tree, const TREEMAP_KEY_TYPE* key) \
    { \
        assert(tree != NULL); \
        TREEMAP_NAME##Iter ret; \
        ret._stack_size = 0; \
        TREEMAP_NAME##Node* current_node = tree->root; \
        while (current_node) { \
            int cmp_res = key ? TREEMAP_KEY_CMP(key, &(current_node->key)) : -1; \
            if (cmp_res == 0 || (!key && !(current_node->_left))) { \
                ret._callstack[(ret._stack_size)++] = current_node; \
                break; \
            } else if (cmp_res < 0) { \
                ret._callstack[(ret._stack_size)++] = current_node; \
                current_node = current_node->_left; \
            } else \
                current_node = current_node->_right; \
        } \
        if (!current_node) { \
            ret._stack_size = 0; \
            ret.current = NULL; \
        } else { \
            ret.current = current_node; \
        } \
        return ret; \
    } \
    \
    \
    /**************************************************************
     * sets the field current to be the next element in the tree,
     * if there are no more elements, current is set to NULL
     **************************************************************/ \
    void TREEMAP_NAME##Iter_inc(TREEMAP_NAME##Iter* iter) \
    { \
        assert(iter != NULL); \
        if (!iter->current) \
            return; \
        TREEMAP_NAME##Node* current_node = (iter->_callstack[--(iter->_stack_size)])->_right; \
        while (current_node) { \
            assert(iter->_stack_size < 198); \
            iter->_callstack[(iter->_stack_size)++] = current_node; \
            current_node = current_node->_left; \
        } \
        iter->current = iter->_stack_size \
            ? iter->_callstack[iter->_stack_size-1] \
            : NULL; \
    }
    



#endif
