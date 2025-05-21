#include <omp.h>
#include <iostream>
#include <set>

int main() 
{
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::set<int> tree;
    int n, d;
    std::cin >> n; 
    double start = omp_get_wtime();
    while (n--) {
        std::cin >> d;
        tree.insert(d);
    }
    double stop = omp_get_wtime();
    printf("insertion took: %lf s\n", stop-start);

    start = omp_get_wtime();
    int sum = 0;
    for (int num : tree) {
        sum += num;
    }
    stop = omp_get_wtime();
    printf("iteration took: %lf s, %d\n", stop-start, sum);
}
