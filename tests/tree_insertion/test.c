#include <assert.h>
#include <stdio.h>
#include <omp.h>

#include "../../datastructures/treemap.h"
#include "../../datastructures/vec.h"

#define CMP(a, b) (*(a) < *(b) ? -1 : (*(a) == *(b) ? 0 : 1))

TREEMAP_DEFINE(Set, int, TREEMAP_NO_VALUE, CMP)
VEC_DEFINE(Vec, int)

int main() 
{
    Set tree = Set_new();
    int n, d;
    scanf("%d", &n);
    Vec input = Vec_new(0);
    for (int i = 0; i < n; i++) {
        scanf("%d", &d);
        Vec_push(&input, d); 
    }

    double start = omp_get_wtime();
    for (int i = 0; i < n; i++) {
        Set_search(&tree, input.arr+i, true);
    }
    double stop = omp_get_wtime();
    printf("insertion took: %lf s\n", stop-start);

    int sum = 0;
    start = omp_get_wtime();
    for (int i = 0; i < n; i++) {
        sum += Set_search(&tree, input.arr+i, false)->key;
    }
    stop = omp_get_wtime();
    printf("queries took: %lf s\n", stop-start);

    start = omp_get_wtime();
    for (SetIter it = Set_get_iter(&tree, NULL); it.current; SetIter_inc(&it)) {
        sum += it.current->key; \
    }
    stop = omp_get_wtime();
    printf("iteration took: %lf s, %d\n", stop-start, sum);

    Set_free(&tree);
    Vec_free(&input);
}
