#include <assert.h>
#include <stdio.h>
#include <omp.h>

#include "../../datastructures/hashmap.h"
#include "../../datastructures/vec.h"

#define EQ(a, b) (*(a) == *(b))
//#define HASH(key) (byte_hasher((const char*)key, 4))
#define HASH(key) (*key)

HASHMAP_DEFINE(Set, int, HASHMAP_NO_VALUE, HASH, EQ)
VEC_DEFINE(Vec, int)

int main() 
{
    Set set = Set_new(0);
    int n, d;
    scanf("%d", &n);
    Vec input = Vec_new(0);
    for (int i = 0; i < n; i++) {
        scanf("%d", &d);
        Vec_push(&input, d); 
    }

    double start = omp_get_wtime();
    for (int i = 0; i < n; i++) {
        Set_search(&set, input.arr+i, true);
    }
    double stop = omp_get_wtime();
    printf("insertion took: %lf s\n", stop-start);

    int sum = 0;
    start = omp_get_wtime();
    for (int i = 0; i < n; i++) {
        sum += Set_search(&set, input.arr+i, false)->key;
    }
    stop = omp_get_wtime();
    printf("queries took: %lf s\n", stop-start);

    start = omp_get_wtime();
    for (int i = 0; i < set._n_buckets; i++) {
        if (set._buckets[i]._is_valid)
            sum += set._buckets[i].key; \
    }
    stop = omp_get_wtime();
    printf("iteration took: %lf s, %d\n", stop-start, sum);

    Set_free(&set);
    Vec_free(&input);
}
