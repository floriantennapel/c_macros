# Efficient generic datastructures in C

This repository contains header-only code for generic datastructures. I have tried to make them easy to use and efficient, following an object oriented design style. 

To use the library, you must first define the functions needed. This is done by calling the <DATASTRUCTURE_NAME>_DEFINE(...) macro. The first argument is always the name of owner struct, this is followed by the type(s) contained in the datastructure and finally any functions needed by the datastructure, for instance a comparison function for treemaps, and hashing function for hashmaps. When passing an argument to a function parameter, you can either pass a function or a macro. 
Keys and values are always stored directly in place, so for very large entries, it might be a good idea to allocate them manually and store pointers in the datastructure.

The 'objects' themselves (memory owning structs), are designed to be lightweight, and can be stored directly on the stack. These typically contain a pointer to the actual data, together with some metadata. Fields prefixed with an underscore should not be accessed.

All of the datastructures are automatically resized, but some of them can be initialized with initial expected capacity for efficiency, this is however not neccessary.

If a set datastructure is needed, simply use a hashmap or treemap, and pass `[HASHMAP|TREEMAP]_NO_VALUE` to the value argument. These are aliases for the empty struct, and use no memory.

### Iterators
Some of the datastructures have functions returning iterators, these are lightweight structs that own no memory. They are used to iterate over entries in the datastructure where this cannot be done trivially. Since they do not own any memory, no deallocation is neccessary after usage.
Iterators moved forwards by the `<DATASTRUCTURE_NAME>Iter_inc` function, and if available moved backwards using the `<DATASTRUCTURE_NAME>Iter_dec` function.

## Example usage 
A simple example using hashmaps, using `char[100]` keys, with `int` entries. All of the datastructures follow this same design pattern.

```C
#include "datastructures/hashmap.h" // header only, linking is not needed
#include <string.h>

// Defining key-type, packing in struct to store in place.
// Another option could be to point to a heap allocated string
typedef struct { char str[100]; } String; 

// should follow signature:
// int (const String*, const String*)
#define KEY_EQ(A, B) (strcmp((A)->str, (B)->str) == 0)

// utilizing pre-defined hash function in hashmap header
#define KEY_HASH(KEY) (byte_hasher((KEY)->str, strlen((KEY)->str)))

// generating functions
// Every function will start with the prefix 'Map'
// and will take a Map* as first argument
HASHMAP_DEFINE(Map, String, int, KEY_HASH, KEY_EQ)

int main()
{
    // new objects must be initialized with _new function
    Map counter1 = Map_new(0); 

    // many different Maps can exist simultaneously
    // expected capacity of 100 elements, will resize if neccessary.
    Map counter2 = Map_new(100);
    
    // ... usage code

    // iterator usage
    for (MapIter it = Map_iter(&counter1); it.current; MapIter_inc(&it)) {
        // ... usage code
    }

    // freeing memory
    Map_free(&counter1);
    Map_free(&counter2);
}
```

---

# API overview

All functions are prefixed with the user given name, however in this overview the prefix is omitted. For instance the function `push`, is actually named `List_push` if defined by `VEC_DEFINE(List, double complex)`.

* [resizeable array](#vech) - [`vec.h`](./datastructures/vec.h)
* [hashmap](#hashmaph) - [`hashmap.h`](./datastructures/hashmap.h)
* [sorted map]() - [`treemap.h`](./datastructures/treemap.h)
* [priority queue]() - [`heap.h`](./datastructures/heap.h)
* [FIFO queue]() - [`queue.h`](./datastructures/queue.h)
* [hashable tuple]() - [`tuple.h`](./tuple.h)

## [`vec.h`](./datastructures/vec.h)
Resizeable array

### Initializer macro
[`VEC_DEFINE(VEC_NAME, VALUE_TYPE)`](./datastructures/vec.h#L21)
* `VEC_NAME`, Name of memory holder and function prefix
* `VALUE_TYPE`, A valid C datatype, it will be stored in place 

### Fields
* `<VALUE_TYPE>* arr`, underlying array, index into this to access elements.
* `size_t size`, number of elements currently stored.

### Functions
* [`<VEC_NAME> new(size_t initial_size)`](./datastructures/vec.h#L35)
* [`<VEC_NAME> copy(const <VEC_NAME>* copy_from)`](./datastructures/vec.h#L51)
* [`void push(<VEC_NAME>* vec, <VALUE_TYPE> value)`](./datastructures/vec.h#L64)
* [`<VALUE_TYPE> pop(<VEC_NAME>* vec)`](./datastructures/vec.h#L81)
* [`void free(<VEC_NAME>* vec)`](./datastructures/vec.h#L100)
* [`void clear(<VEC_NAME>* vec)`](./datastructures/vec.h#L112)

## [`hashmap.h`](./datastructures/hashmap.h)
Unordered associative array. Keys and values are stored together in structs of type `<HASHMAP_NAME>Entry`.

### Initializer macro
[`HASHMAP_DEFINE(HASHMAP_NAME, KEY_TYPE, VALUE_TYPE, HASH_FUNC, KEY_EQ_FUNC)`](./datastructures/hashmap.h#L74)

* `HASHMAP_NAME`, Name of memory holder and function prefix.
* `KEY_TYPE`, A valid C type, stored in place.
* `VALUE_TYPE`, A valid C type, stored in place. Pass `HASHMAP_NO_VALUE` for a hashset. 
* `HASH_FUNC`, Function or macro to hash keys. Must have signature:

    `size_t HASH_FUNC(const <KEY_TYPE>*)`
* `KEY_EQ_FUNC`, Function or macro to check if two keys are equal. Must return 1 if keys are equal and 0 otherwise. Signature:

    `bool KEY_EQ_FUNC(const <KEY_TYPE>*, const <KEY_TYPE>*)`

### Fields
* `size_t size`, number of elements currently stored. 

### Functions 
* [`static size_t byte_hasher(const char* byte_array, size_t n_bytes`](./datastructures/hashmap.h#L17)
* [`<HASHMAP_NAME> new(size_t initial_capacity)`](./datastructures/hashmap.h#L101)
* [`<HASHMAP_NAME>Entry* search(<HASHMAP_NAME>* map, const <KEY_TYPE>* key, bool insert)`](./datastructures/hashmap.h#L162)
* [`void insert(<HASHMAP_NAME>* map, <KEY_TYPE> key, <VALUE_TYPE> value)`](./datastructures/hashmap.h#L185)
* [`bool contains(const <HASHMAP_NAME>* map, const <KEY_TYPE>* key)`](./datastructures/hashmap.h#L194)
* [`void free(<HASHMAP_NAME>* map)`](./datastructures/hashmap.h#L204)
* [`void remove(<HASHMAP_NAME>* map, const <KEY_TYPE>* key)`](./datastructures/hashmap.h#L216)
* [`<HASHMAP_NAME>Iter iter(const <HASHMAP_NAME>* map)`](./datastructures/hashmap.h#L268)

## [`treemap.h`](./datastructures/treemap.h)
### Initializer macro
### Fields
### Functions

## [`heap.h`](./datastructures/heap.h)
### Initializer macro
### Fields
### Functions

## [`queue.h`](./datastructures/queue.h)
### Initializer macro
### Fields
### Functions

## [`tuple.h`](./datastructures/tuple.h)
### Initializer macro
### Fields
### Functions
