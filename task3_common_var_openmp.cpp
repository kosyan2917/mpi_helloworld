#include <iostream>
#include <omp.h>
int main(int argc, char * argv[] )
{
    int sharedVariable = 0;
    for (int i = 0; i < omp_get_max_threads(); i++) {
        #pragma omp parallel
        {
            int threadId = omp_get_thread_num();
            #pragma omp critical
            {
                if (threadId == i) {

                    sharedVariable++;
                    std::cout << "Thread " << threadId << ": Shared Variable = " << sharedVariable << std::endl;
                }
            }
        }
    }
    return 0;
}
