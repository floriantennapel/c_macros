#include <omp.h>
#include <iostream>
#include <set>
#include <vector>

int main() 
{
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::set<int> tree;
    std::vector<int> input;
    int n, d;
    std::cin >> n; 
    for (int i = 0; i < n; i++) {
        std::cin >> d;
        input.push_back(d);
    }

    double start = omp_get_wtime();
    for (int i = 0; i < n; i++)
        tree.insert(input[i]);
    double stop = omp_get_wtime();
    printf("insertion took: %lf s\n", stop-start);

    int sum = 0;
    start = omp_get_wtime();
    for (int i = 0; i < n; i++)
        sum += *tree.find(input[i]);
    stop = omp_get_wtime();
    printf("queries took: %lf s\n", stop-start);


    start = omp_get_wtime();
    for (int num : tree) {
        sum += num;
    }
    stop = omp_get_wtime();
    printf("iteration took: %lf s, %d\n", stop-start, sum);
}
