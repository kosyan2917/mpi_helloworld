#include <iostream>
#include <omp.h>
#include <unistd.h>

void without_schedule() {
    int i;
    printf("Running for without schedule \n");
    #pragma omp parallel for
    for (i = 0; i < 65; i++) {
        printf("Thread %d is running number %d\n", omp_get_thread_num(), i);
    }

}

void schedule_static_without_batch_size() {
    int i;
    printf("Running for with static schedule without batch size \n");
    #pragma omp parallel for schedule(static)
    for (i = 0; i < 65; i++) {
        printf("Thread %d is running number %d\n", omp_get_thread_num(), i);
    }
}

void schedule_static(int batch_size) {
    int i;
    printf("Running for with static schedule with batch size = %d \n", batch_size);
    #pragma omp parallel for schedule(static, batch_size)
    for (i = 0; i < 65; i++) {
        printf("Thread %d is running number %d\n", omp_get_thread_num(), i);
    }
}

void schedule_dynamic(int batch_size) {
    int i;
    printf("Running for with dynamic schedule with batch size = %d \n", batch_size);
    #pragma omp parallel for schedule(dynamic, batch_size)
    for (i = 0; i < 65; i++) {
        sleep(1);
        printf("Thread %d is running number %d\n", omp_get_thread_num(), i);
    }
}

void schedule_guided(int batch_size) {
    int i;
    printf("Running for with guided schedule with batch size = %d \n", batch_size);
    #pragma omp parallel for schedule(guided, batch_size)
    for (i = 0; i < 65; i++) {
        printf("Thread %d is running number %d\n", omp_get_thread_num(), i);
    }
}


int main(int argc, char * argv[] )
{
    omp_set_num_threads(4);
    without_schedule();
    printf("__________________________________\n");
    schedule_static_without_batch_size();
    printf("__________________________________\n");
    schedule_static(1);
    printf("__________________________________\n");
    schedule_static(4);
    printf("__________________________________\n");
    schedule_dynamic(1);
    printf("__________________________________\n");
    schedule_dynamic(4);
    printf("__________________________________\n");
    schedule_guided(1);
    printf("__________________________________\n");
    schedule_guided(4);
    return 0;
}

