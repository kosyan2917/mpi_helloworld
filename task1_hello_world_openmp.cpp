#include <iostream>
#include <omp.h>
int main(int argc, char * argv[] )
{
    int thread_num = 4;
    omp_set_num_threads(thread_num);
    #pragma omp parallel
    {
        printf("Hello world! %d %d \n", omp_get_thread_num(), omp_get_max_threads());
    }
    return 0;
}