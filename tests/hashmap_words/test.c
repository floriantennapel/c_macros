#include <assert.h>
#include <stdio.h>
#include <omp.h>

#include "../../datastructures/hashmap.h"
#include "../../datastructures/vec.h"

#define MAX_STR_LEN 200

typedef struct
{
    char chars[MAX_STR_LEN];
    int size;
    size_t hash;
} String;

#define EQ(a, b) (strcmp((*(a))->chars, (*(b))->chars) == 0)
#define HASH(key) ((*(key))->hash)

size_t java_hash(const String** str)
{
    int h = 0;
    for (int i = 0; i < (*str)->size; i++) {
        char v = (*str)->chars[i];
        h = 31 * h + (v & 0xff);
    }
    return h;
}

HASHMAP_DEFINE(Set, String*, HASHMAP_NO_VALUE, HASH, EQ)
VEC_DEFINE(Vec, String)

int main() 
{
    Set set = Set_new(0);
    Vec input = Vec_new(0);
    String buf;
    while (fgets(buf.chars, MAX_STR_LEN, stdin)) {
        buf.size = strlen(buf.chars)-1;
        buf.chars[buf.size] = '\0';
        buf.hash = byte_hasher(buf.chars, buf.size);
        Vec_push(&input, buf); 
    }
    int n = input.size;

    double start = omp_get_wtime();
    for (int i = 0; i < n; i++) {
        const String* key = input.arr+i; 
        Set_search(&set, &key, true);
    }
    double stop = omp_get_wtime();
    printf("insertion took: %lf s\n", stop-start);

    int sum = 0;
    start = omp_get_wtime();
    for (int i = 0; i < n; i++) {
        const String* key = input.arr+i; 
        sum += Set_search(&set, &key, false)->key->chars[0];
    }
    stop = omp_get_wtime();
    printf("queries took: %lf s\n", stop-start);

    start = omp_get_wtime();
    for (int i = 0; i < set._n_buckets; i++) {
        if (set._buckets[i]._is_valid)
            sum += set._buckets[i].key->chars[0]; \
    }
    stop = omp_get_wtime();
    printf("iteration took: %lf s, %d\n", stop-start, sum);

    Set_free(&set);
    Vec_free(&input);
}
