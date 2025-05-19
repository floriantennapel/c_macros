
/******************************************************************************
 * Runs a benchmark comparing libc's builting qsort to a simple heap-sort 
 * Generate the dataset before running using the nums_generator python script
 *
 * Tests simple behaviour of vectors and heap functions
 ******************************************************************************/

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <stdbool.h>

#include "../datastructures/vec.h"
#include "../datastructures/heap.h"


#define CMP(a, b) (*(a) > *(b) ? -1 : (*(a) < *(b) ? 1 : 0))

int cmp(const void* a, const void* b) 
{
    int* ai = (int*)a;
    int* bi = (int*)b;
    return *ai < *bi ? -1 : (*ai > *bi ? 1 : 0);
}

VEC_DEFINE(Vector, int)
HEAP_DEFINE(Heap, Vector, int, CMP)

/**
* ensures that sorting is not optimized away
* overflow will probably occur
* */
int sum_vec(const Vector* vec) 
{
    int sum = 0;
    for (size_t i = 0; i < vec->size; i++) {
        sum += vec->arr[i];
    }
    return sum;
}

bool is_sorted(const Vector* vec)
{
    int prev = vec->arr[0];
    for (int i = 1; i < vec->size; i++) {
        int curr = vec->arr[i];
        if (curr < prev)
            return false;
        prev = curr;
    }
    return true;
}

int main()
{
    Vector qsort_vec = Vector_new(0);
    Vector heap_sort_vec = Vector_new(0);

    FILE* nums_file = fopen("tests/nums.txt", "r");
    size_t n;
    int d;
    fscanf(nums_file, "%lu", &n);
    for (size_t i = 0; i < n; i++) {
        fscanf(nums_file, "%d", &d);
        Vector_push(&qsort_vec, d);
        Vector_push(&heap_sort_vec, d);
    }
    fclose(nums_file);
    assert(qsort_vec.size == n && heap_sort_vec.size == n);

    //qsort
    clock_t start = clock();
    qsort(qsort_vec.arr, qsort_vec.size, sizeof(int), cmp);
    clock_t end = clock();
    printf("qsort took %lf s\n", (double)(end - start) / CLOCKS_PER_SEC);
    assert(is_sorted(&qsort_vec));


    // heap sort
    start = clock();
    Heap_heapify(&heap_sort_vec);
    int tmp;
    while (heap_sort_vec.size > 1) {
        int tmp = heap_sort_vec.arr[0];
        heap_sort_vec.arr[0] = heap_sort_vec.arr[heap_sort_vec.size - 1];
        heap_sort_vec.arr[heap_sort_vec.size - 1] = tmp;
        heap_sort_vec.size--;
        _Heap_sift_down(&heap_sort_vec, 0);
    }
    heap_sort_vec.size = n;
    end = clock();
    printf("heapsort took %lf s\n", (double)(end - start) / CLOCKS_PER_SEC);
    assert(is_sorted(&heap_sort_vec));
    assert(sum_vec(&qsort_vec) == sum_vec(&heap_sort_vec));

    //freeing used memory
    free(heap_sort_vec.arr);
    free(qsort_vec.arr);
}
