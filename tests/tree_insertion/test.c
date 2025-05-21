#include <assert.h>
#include <stdio.h>
#include <omp.h>
#include "datastructures/treemap.h"

#define CMP(a, b) (*(a) < *(b) ? -1 : (*(a) == *(b) ? 0 : 1))

TREEMAP_DEFINE(Set, int, TREEMAP_NO_VALUE, CMP)

/*void print_tree(TreeNode* node, int depth) 
{
    if (!node)
        return;
    for (int i = 0; i < depth; i++) {
        printf("  ");
    }
    printf("%d: %u\n", node->key, node->_color);
    print_tree(node->_left, depth+1);
    print_tree(node->_right, depth+1);
}*/

int main() 
{
    Set tree = Set_new();
    int n, d;
    scanf("%d", &n);
    double start = omp_get_wtime();
    while (n--) {
        scanf("%d", &d);
        Set_search(&tree, &d, true);
        assert(Set_search(&tree, &d, false));
    }
    double stop = omp_get_wtime();
    printf("insertion took: %lf s\n", stop-start);

    start = omp_get_wtime();
    int sum = 0;
    for (SetIter iter = Set_get_iter(&tree, NULL); iter.current; SetIter_inc(&iter)) {
        sum += iter.current->key;
    }
    stop = omp_get_wtime();
    printf("iteration took: %lf s, %d\n", stop-start, sum);
}
