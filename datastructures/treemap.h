#ifndef TREEMAP_H
#define TREEMAP_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct
{} TREEMAP_NO_VALUE;

// maximum number of children
#ifndef _TREEMAP_M
#define _TREEMAP_M 32
#endif

#define TREEMAP_DEFINE(TREEMAP_NAME, TREEMAP_KEY_TYPE, TREEMAP_VAL_TYPE, TREEMAP_KEY_CMP) \
    typedef struct \
    { \
        TREEMAP_KEY_TYPE key; \
        TREEMAP_VAL_TYPE value; \
    } TREEMAP_NAME##Entry; \
    \
    typedef struct _##TREEMAP_NAME##Node _##TREEMAP_NAME##Node; \
    \
    typedef struct \
    { \
        TREEMAP_NAME##Entry entry; \
        _##TREEMAP_NAME##Node* lt_child; \
    } _##TREEMAP_NAME##NodeEntry; \
    \
    struct _##TREEMAP_NAME##Node \
    { \
        _##TREEMAP_NAME##NodeEntry entries[_TREEMAP_M]; \
        unsigned n_entries: 15; \
        unsigned is_leaf: 1; \
    }; \
    \
    typedef struct \
    { \
        _##TREEMAP_NAME##Node* _root; \
        size_t size; \
    } TREEMAP_NAME; \
    \
    typedef struct \
    { \
    _##TREEMAP_NAME##Node* node; \
    unsigned node_ind: 16; \
    } _##TREEMAP_NAME##IterStackEntry; \
    \
    typedef struct \
    { \
        TREEMAP_NAME##Entry* current; \
        unsigned _stack_size: 8; \
        _##TREEMAP_NAME##IterStackEntry _callstack[200]; \
    } TREEMAP_NAME##Iter; \
    \
    \
    /**********************************************************************************
     * Initializes a new treemap
     *
     * The returned struct is the owner of the tree
     * The field `size` can be used to query the number of entries stored in the tree
     **********************************************************************************/ \
    TREEMAP_NAME TREEMAP_NAME##_new() \
    { \
        _##TREEMAP_NAME##Node* root = calloc(1, sizeof(_##TREEMAP_NAME##Node)); \
        assert(root != NULL); \
        root->is_leaf = 1; \
        return (TREEMAP_NAME) {root, 0}; \
    } \
    \
    \
    /*******************************************************************************
     * Do not use this function
     *
     * Recursively searches for (and if not present inserts) an entry int the tree
     *******************************************************************************/ \
    _##TREEMAP_NAME##NodeEntry* _##TREEMAP##_search_helper( \
        TREEMAP_NAME* map, _##TREEMAP_NAME##Node* node, const TREEMAP_KEY_TYPE* key, \
        bool insert, TREEMAP_NAME##Entry** res) \
    { \
        int n = node->n_entries; \
        for (int i = 0; i < n+1; i++) { \
            _##TREEMAP_NAME##NodeEntry* entry = node->entries+i; \
            int cmp_res = i == n \
                    ? -1 \
                    : TREEMAP_KEY_CMP((const TREEMAP_KEY_TYPE*)(key), (const TREEMAP_KEY_TYPE*)(&(entry->entry.key))); \
            \
            if (cmp_res == 0) { \
                *res = &(entry->entry); \
                return NULL; \
            } else if (cmp_res < 0) { \
                bool set_res = false; \
                \
                _##TREEMAP_NAME##NodeEntry new_entry; \
                if (node->is_leaf) { \
                    if (!insert) { \
                        *res = NULL; \
                        return NULL; \
                    } \
                    new_entry.entry.key = *key; \
                    new_entry.lt_child = NULL; \
                    set_res = true; \
                    (map->size)++; \
                } else { \
                    _##TREEMAP_NAME##NodeEntry* floater = _##TREEMAP##_search_helper( \
                            map, entry->lt_child, key, insert, res \
                    ); \
                    if (!floater) \
                        return NULL; \
                    if (&floater->entry == *res) \
                        set_res = true; \
                    new_entry = *floater; \
                } \
                /* if we got here it means we have a new entry to insert */ \
                \
                /* storing gt_child in case memmove overwrites it */ \
                _##TREEMAP_NAME##Node* gt_child = (node->entries+n)->lt_child; \
                bool is_full = n == _TREEMAP_M - 1; \
                int n_move = is_full \
                        ? n - i \
                        : n - i + 1; \
                memmove(node->entries+i+1, node->entries+i, n_move*sizeof(_##TREEMAP_NAME##NodeEntry)); \
                node->entries[i] = new_entry; \
                \
                if (!is_full) { \
                    if (set_res) \
                        *res = &(node->entries+i)->entry; \
                    (node->n_entries)++; \
                    return NULL; \
                } \
                int median_ind = n / 2; \
                _##TREEMAP_NAME##NodeEntry median = node->entries[median_ind]; \
                _##TREEMAP_NAME##Node* new_node = calloc(1, sizeof(_##TREEMAP_NAME##Node)); \
                new_node->is_leaf = node->is_leaf; \
                (new_node->entries+median_ind)->lt_child = median.lt_child; \
                median.lt_child = new_node; \
                memcpy(new_node->entries, node->entries, sizeof(_##TREEMAP_NAME##NodeEntry)*median_ind); \
                memmove(node->entries, node->entries+median_ind+1, sizeof(_##TREEMAP_NAME##NodeEntry)*(_TREEMAP_M/2)); \
                (node->entries+_TREEMAP_M/2)->lt_child = gt_child; \
                node->n_entries = _TREEMAP_M/2; \
                new_node->n_entries = median_ind; \
                \
                if (set_res) { \
                    if (i < median_ind) \
                        *res = &(new_node->entries+i)->entry; \
                    else if (i > median_ind) \
                        *res = &(node->entries+(i-median_ind-1))->entry; \
                    else { \
                        /* must reassign res recursively if this is the case */ \
                        *res = &(new_node->entries+n)->entry; \
                    } \
                } \
                /* temporarily store median in last index of left child */ \
                new_node->entries[n] = median; \
                return new_node->entries + n; \
            } \
        } \
        fprintf(stderr, #TREEMAP_NAME"_search: runtime error\n"); \
        exit(1); \
        return NULL; /* just to avoid compiler warning */\
    } \
    \
    \
    /*************************************************************************
     * Searches for a key-value pair in the tree
     * 
     * @param insert is this is true and the entry is not present
     *     a new entry is inserted, with a value set to zeroes
     *
     * @returns A pointer to the entry containing the key and value, 
     *    THE KEY MUST NOT BE MODIFIED
     *    If the entry was not found, and insert was false, NULL is returned
     *************************************************************************/ \
    TREEMAP_NAME##Entry* TREEMAP_NAME##_search(TREEMAP_NAME* map, const TREEMAP_KEY_TYPE* key, bool insert) \
    { \
        assert(map != NULL); \
        assert(key != NULL); \
        TREEMAP_NAME##Entry* res; \
        _##TREEMAP_NAME##NodeEntry* floater = _##TREEMAP##_search_helper( \
                map, map->_root, key, insert, &res \
        ); \
        if (floater) { \
            _##TREEMAP_NAME##Node* old_root = map->_root; \
            _##TREEMAP_NAME##Node* new_root = calloc(1, sizeof(_##TREEMAP_NAME##Node)); \
            assert(new_root); \
            new_root->is_leaf = 0; \
            new_root->n_entries = 1; \
            new_root->entries[0] = *floater; \
            new_root->entries[1].lt_child = old_root; \
            new_root->n_entries = 1; \
            if (&floater->entry == res) \
                res = &(new_root->entries->entry); \
            map->_root = new_root; \
        } \
        return res; \
    } \
    \
    \
    /****************************************
     * Deallocate resources used by treemap
     * DO NOT use it after this point
     ****************************************/ \
    void TREEMAP_NAME##_free(TREEMAP_NAME* map) \
    { \
        assert(map); \
        _##TREEMAP_NAME##Node* stack[200]; \
        int stack_size = 0; \
        stack[stack_size++] = map->_root; \
        while (stack_size) { \
            _##TREEMAP_NAME##Node* current = stack[--stack_size]; \
            if (!current->is_leaf) { \
                for (int i = 0; i < current->n_entries+1; i++) \
                    stack[stack_size++] = current->entries[i].lt_child; \
            } \
            free(current); \
        } \
    } \
    \
    \
    /*************************************
     * Check if key is contained in tree
     *************************************/ \
    bool TREEMAP_NAME##_contains(const TREEMAP_NAME* map, const TREEMAP_KEY_TYPE* key) \
    { \
        assert(map != NULL); \
        assert(key != NULL); \
        return TREEMAP_NAME##_search((TREEMAP_NAME*)map, key, false) != NULL; \
    } \
    \
    \
    /******************************************************************
     * Set the value at the given key
     * If the key is not present in the tree, a new entry is inserted
     ******************************************************************/ \
    void TREEMAP_NAME##_insert(TREEMAP_NAME* map, TREEMAP_KEY_TYPE key, TREEMAP_VAL_TYPE value) \
    { \
        assert(map != NULL); \
        TREEMAP_NAME##_search(map,(const TREEMAP_KEY_TYPE*)&key, true)->value = value; \
    } \
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
        _##TREEMAP_NAME##Node* current_node = tree->_root; \
        while (current_node) { \
            for (int i = 0; i < current_node->n_entries+1; i++) { \
                _##TREEMAP_NAME##NodeEntry* entry = current_node->entries+i; \
                int cmp_res = !key || i == current_node->n_entries \
                        ? -1 \
                        : TREEMAP_KEY_CMP(key, (const TREEMAP_KEY_TYPE*)(&(entry->entry.key))); \
                if (cmp_res > 0) \
                    continue; \
                \
                if (i != current_node->n_entries) \
                    ret._callstack[(ret._stack_size)++] = (_##TREEMAP_NAME##IterStackEntry) {current_node, i}; \
                if (cmp_res == 0 || (!key && current_node->is_leaf)) \
                    goto break_outer; \
                else { \
                    if (!current_node->is_leaf) \
                        current_node = entry->lt_child; \
                    else \
                        current_node = NULL; \
                    break; \
                } \
            } \
        } \
    break_outer: \
        if (!current_node) { \
            ret._stack_size = 0; \
            ret.current = NULL; \
        } else { \
            _##TREEMAP_NAME##IterStackEntry se = ret._callstack[ret._stack_size-1]; \
            ret.current = &(se.node->entries+(se.node_ind))->entry; \
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
        _##TREEMAP_NAME##IterStackEntry se = iter->_callstack[--(iter->_stack_size)]; \
        _##TREEMAP_NAME##Node* current_node = se.node; \
        int current_ind = se.node_ind + 1; \
        for (;;) { \
            if (current_ind != current_node->n_entries) \
                iter->_callstack[(iter->_stack_size)++] = (_##TREEMAP_NAME##IterStackEntry) {current_node, current_ind}; \
            if (current_node->is_leaf) \
                break; \
            current_node = current_node->entries[current_ind].lt_child; \
            current_ind = 0; \
        } \
        \
        se = iter->_callstack[iter->_stack_size-1]; \
        iter->current = iter->_stack_size \
            ? &(se.node->entries[se.node_ind].entry) \
            : NULL; \
    }


#endif
