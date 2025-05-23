#ifndef HASHMAP_H
#define HASHMAP_H

#include <stddef.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "siphash.h"

/**********************************************************************************
 * Utility function to hash byte arrays
 * Can for instance be used to hash structs by reinterpreting them as byte-arrays
 **********************************************************************************/
static size_t byte_hasher(const char* byte_array, size_t n_bytes) 
{
    uint64_t key[] = {0, 0};  // no key is used
    uint64_t output;
    _siphash_source_code(byte_array, n_bytes, key, (uint8_t*)&output, 8);
    return output;
}

/************************************
 * Utility function to hash strings
 ************************************/
static size_t string_hasher(const char* str) 
{
    size_t len = strlen(str);
    return byte_hasher(str, len);
}

/*******************************
 * Empty value to use for sets
 *******************************/
typedef struct
{} HASHMAP_NO_VALUE;

// constants taken from openJDK HashMap.java
#ifndef _HASHMAP_MIN_BUCKET_ARRAY_SIZE
#define _HASHMAP_MIN_BUCKET_ARRAY_SIZE 16 
#endif

#define _HASHMAP_MAX_BUCKET_ARRAY_SIZE (1 << 30)

#ifndef _HASHMAP_LOAD_FACTOR
#define _HASHMAP_LOAD_FACTOR 0.6
#endif

/*******************************************************************************************************************
 * Generates functions for a HashMap                                                                               *
 *                                                                                                                 *
 * All keys and values are stored in place                                                                         *
 * Resizing is done automatically when the load factor is surpassed                                                *
 *                                                                                                                 *
 * The load factor and minimum capacity can be changed by setting                                                  *
 * the corresponding macros before including this header                                                           *
 *                                                                                                                 *
 * _HASHMAP_LOAD_FACTOR should however not be set to any values higheer than 0.75                                  *
 * as this will severly impact performance                                                                         *
 *                                                                                                                 *
 * @param HASHMAP_NAME name of owner struct and prefix of generated functions                                      *
 *                                                                                                                 *
 * @param HASHMAP_KEY_TYPE stored in place and not separately allocated.                                           *
 *    - This means that any array-type keys should be wrapped in a struct.                                         *
 *    For instance given char* keys with a known max length of 50, use `struct {char chars[50];}` as key-type      *
 *    - It is also possible to use pointers as keys, they however need to be stored outside of the HashMap itself  *
 *                                                                                                                 *
 * @param HASHMAP_VALUE_TYPE type of entries, for a Set with no entries, simply pass HASHMAP_NO_VALUE.             *
 *                                                                                                                 *
 * @param HASHMAP_HASH_FUNC function or macro to hash keys                                                         * 
 *      signature: `size_t (*)(const HASHMAP_KEY_TYPE*)`                                                           *
 *    must follow typical rules for hashing:                                                                       *
 *    - All keys that are equal according to `HASHMAP_KEY_EQ_FUNC` must also hash to the same value                *
 *    - The same key should always produce the same hash                                                           *
 *    - Two unequal keys may hash to the same value                                                                *
 *                                                                                                                 *
 * @param HASHMAP_KEY_EQ_FUNC function or macro to compare equality of two keys.                                   *
 *    signature: `bool (*)(const HASHMAP_KEY_TYPE*, const HASHMAP_KEY_TYPE*)`                                      *
 *    Any keys that are found to be equal *MUST* also hash to the same value                                       *
 *******************************************************************************************************************/
#define HASHMAP_DEFINE(HASHMAP_NAME, HASHMAP_KEY_TYPE, HASHMAP_VALUE_TYPE, HASHMAP_HASH_FUNC, HASHMAP_KEY_EQ_FUNC) \
    typedef struct \
    { \
        unsigned _is_valid: 1; \
        HASHMAP_KEY_TYPE key; \
        HASHMAP_VALUE_TYPE value; \
    } HASHMAP_NAME##Entry; \
    \
    typedef struct \
    { \
        size_t size; \
        HASHMAP_NAME##Entry* _buckets; \
        size_t _n_buckets; \
    } HASHMAP_NAME; \
    \
    \
    /********************************************************************************************************************
     * Creates a new HashMap
     *
     * @param initial_capacity should be set to the expected number of entries to avoid excessive rehashing of entries, 
     *    it can however be set to any value, as the HashMap is resized automatically as needed
     ********************************************************************************************************************/ \
    HASHMAP_NAME HASHMAP_NAME##_new(size_t initial_capacity) \
    { \
        initial_capacity /= _HASHMAP_LOAD_FACTOR; \
        size_t capacity = _HASHMAP_MIN_BUCKET_ARRAY_SIZE; \
        for (; capacity < initial_capacity && capacity < _HASHMAP_MAX_BUCKET_ARRAY_SIZE; capacity <<= 1); \
        HASHMAP_NAME ret = {0, NULL, capacity}; \
        ret._buckets = calloc(capacity, sizeof(HASHMAP_NAME##Entry)); \
        assert(ret._buckets); \
        return ret; \
    } \
    \
    \
    /****************************************************
     * Do not use this function
     *
     * Finds the place where an entry is/can be stored.
     ****************************************************/ \
    HASHMAP_NAME##Entry* _##HASHMAP_NAME##_locate_entry_holder(HASHMAP_NAME* map, const HASHMAP_KEY_TYPE* key) \
    { \
        assert(map); \
        assert(key); \
        size_t hash = (HASHMAP_HASH_FUNC(key)) % map->_n_buckets; \
        size_t ind = hash; \
        for (int i = 0;;i++) { \
            if (!((map->_buckets+ind)->_is_valid) || (HASHMAP_KEY_EQ_FUNC((key), ((const HASHMAP_KEY_TYPE*) &((map->_buckets+ind)->key))))) {\
                printf("%d collisions\n", i); \
                break; \
            } \
            ind = (ind + 1) % map->_n_buckets; \
        } \
        return map->_buckets + ind; \
    } \
    \
    \
    /*************************************************
     * Do not use this function
     *
     * resizes bucket array and rehashes all entries
     *************************************************/ \
    void _##HASHMAP_NAME##_resize(HASHMAP_NAME* map, size_t new_size) \
    { \
        size_t old_n_buckets = map->_n_buckets; \
        HASHMAP_NAME##Entry* old_buckets = map->_buckets; \
        map->_n_buckets = new_size; \
        map->_buckets = calloc(map->_n_buckets, sizeof(HASHMAP_NAME##Entry)); \
        assert(map->_buckets); \
        \
        for (size_t i = 0; i < old_n_buckets; i++) { \
            HASHMAP_NAME##Entry* entry = old_buckets+i; \
            if (entry->_is_valid) \
                *(_##HASHMAP_NAME##_locate_entry_holder(map, (const HASHMAP_KEY_TYPE*) &(entry->key))) = *entry; \
        } \
        free(old_buckets); \
    } \
    \
    \
    /***************************************************************************************************************
     * Finds the corresponding entry (key-value pair) searching according to the key
     *
     * @param insert If an entry does not already exist in the HashMap it will be inserted if this is set to true.
     *    In case of a new insertion, the value is not set and needs to be set by the caller afterwards
     *    If it is false and the entry does not exist this function will return NULL
     ***************************************************************************************************************/ \
    HASHMAP_NAME##Entry* HASHMAP_NAME##_search(HASHMAP_NAME* map, const HASHMAP_KEY_TYPE* key, bool insert) \
    { \
        assert(map); \
        assert(key); \
        \
        if ((map->size) / (double) map->_n_buckets >= _HASHMAP_LOAD_FACTOR) \
            _##HASHMAP_NAME##_resize(map, map->_n_buckets * 2); \
        \
        HASHMAP_NAME##Entry* entry_holder = _##HASHMAP_NAME##_locate_entry_holder(map, key); \
        if (insert && !(entry_holder->_is_valid)) { \
            entry_holder->_is_valid = 1; \
            entry_holder->key = (HASHMAP_KEY_TYPE) *key; \
            map->size++; \
        } \
        \
        return entry_holder->_is_valid ? entry_holder : NULL; \
    } \
    \
    \
    /**********************************
     * Assigns a value to a given key
    ***********************************/ \
    void HASHMAP_NAME##_insert(HASHMAP_NAME* map, const HASHMAP_KEY_TYPE* key, const HASHMAP_VALUE_TYPE* value) \
    { \
        HASHMAP_NAME##_search(map, key, true)->value = *value; \
    } \
    \
    \
    /*********************************************
     * Checks if a key is present in the HashMap
    **********************************************/ \
    bool HASHMAP_NAME##_contains(const HASHMAP_NAME* map, const HASHMAP_KEY_TYPE* key) \
    { \
        return HASHMAP_NAME##_search((HASHMAP_NAME*) map, key, false) != NULL; \
    } \
    \
    \
    /**************************************************
    * Deallocates all resources used by this HashMap.
    * It must not be used after this point
    ***************************************************/ \
    void HASHMAP_NAME##_free(HASHMAP_NAME* map) \
    { \
        free(map->_buckets); \
    } \
    \
    \
    /**************************************************************************
     * Removes an entry from the HashMap 
     *
     * Other entries might be rehashed, 
     * so take care to not call this function while iterating over `_buckets`
     **************************************************************************/ \
    void HASHMAP_NAME##_remove(HASHMAP_NAME* map, const HASHMAP_KEY_TYPE* key) \
    { \
        assert(map); \
        assert(key); \
        HASHMAP_NAME##Entry* entry_holder = _##HASHMAP_NAME##_locate_entry_holder(map, key); \
        if (!(entry_holder->_is_valid)) \
            return; \
        entry_holder->_is_valid = 0; \
        (map->size)--; \
        \
        /* cleanup */ \
        size_t ind = (entry_holder - map->_buckets + 1) % map->_n_buckets; \
        for (;map->_buckets[ind]._is_valid; ind = (ind + 1) % map->_n_buckets) { \
            HASHMAP_NAME##Entry entry = map->_buckets[ind]; \
            map->_buckets[ind]._is_valid = 0; \
            *(_##HASHMAP_NAME##_locate_entry_holder(map, (const HASHMAP_KEY_TYPE*) &(entry.key))) = entry; \
        } \
        if (4 * map->size / (double) map->_n_buckets < _HASHMAP_LOAD_FACTOR && !(map->_n_buckets <= _HASHMAP_MIN_BUCKET_ARRAY_SIZE)) \
            _##HASHMAP_NAME##_resize(map, map->_n_buckets / 2); \
        \
    }

#endif

