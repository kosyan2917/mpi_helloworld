#include <stdio.h>
#include <omp.h>

int a = 12345;
#pragma omp threadprivate(a)

void copyprivate_demo() {
    int pr = 123;

    #pragma omp parallel default(none) firstprivate(pr)
    {
        printf("Thread %d: pr = %d.\n", omp_get_thread_num(), pr);

        #pragma omp barrier

        #pragma omp single copyprivate(pr)
        {
            pr = 456;
            printf("Thread %d executes the single construct and changes pr to %d.\n", omp_get_thread_num(), pr);
        }

        printf("Thread %d: pr = %d.\n", omp_get_thread_num(), pr);
    }
}

void copyin_demo() {
    #pragma omp parallel
    {
        #pragma omp master
        {
            printf("[First parallel region] Master thread changes the value of a to 67890.\n");
            a = 67890;
        }
        #pragma omp barrier
        printf("[First parallel region] Thread %d: a = %d.\n", omp_get_thread_num(), a);
    }

    #pragma omp parallel copyin(a)
    {
        printf("[Second parallel region] Thread %d: a = %d.\n", omp_get_thread_num(), a);
    }
}


int main(int argc, char* argv[])
{
    omp_set_dynamic(0);
    copyin_demo();
    printf("__________________________________\n");
    copyprivate_demo();
    return 0;
}