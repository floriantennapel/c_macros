#include <omp.h>
#include <iostream>
#include <unordered_set>
#include <vector>
#include <string>

int main() 
{
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::unordered_set<std::string> set;
    std::vector<std::string> input;
    std::string s;
    while ((std::cin >> s)) {
        input.push_back(s);
    }
    int n = input.size();

    double start = omp_get_wtime();
    for (int i = 0; i < n; i++)
        set.insert(input[i]);
    double stop = omp_get_wtime();
    printf("insertion took: %lf s\n", stop-start);

    int sum = 0;
    start = omp_get_wtime();
    for (int i = 0; i < n; i++)
        sum += (*set.find(input[i]))[0];
    stop = omp_get_wtime();
    printf("queries took: %lf s\n", stop-start);


    start = omp_get_wtime();
    for (auto& s : set) {
        sum += s[0];
    }
    stop = omp_get_wtime();
    printf("iteration took: %lf s, %d\n", stop-start, sum);
}
