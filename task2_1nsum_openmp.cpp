#include <iostream>
#include <omp.h>
#include <cstdlib>

int main(int argc, char * argv[] )
{
    if (argc != 3) {
        std::cout << "Неправильное число аргументов\n";
        return 0;
    }
    long n = strtol(argv[1], NULL, 10);
    long thread_num = strtol(argv[2], NULL, 10);
    omp_set_num_threads(thread_num);
    int i;
    float sum = 0;
    #pragma omp parallel for reduction(+:sum)
    for (i = 1; i <= n; i++) {
        sum += 1.0/i;
    }
    std::cout << sum << "\n";
    return 0;

}