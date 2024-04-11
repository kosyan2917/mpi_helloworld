#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <sys/sysinfo.h>
#include <pthread.h>
#include <sys/time.h>

struct search_args {
    int* data;
    long beg;
    long end;
    int value;
    long index;
};

void* simple_search(void* args) {
    struct search_args *a = (struct search_args*)args;
    for (long i = a->beg; i < a->end; i++)
        if (a->data[i] == a->value) {
            a->index = i;
            return a;
        }
    a->index = -1;
    return NULL;
}

long int_search(int* data, long size, int nthreads) {
    pthread_t *threads = (pthread_t*)malloc(nthreads * sizeof(pthread_t));
    struct search_args *args = (struct search_args*)malloc(nthreads * sizeof(struct search_args));

    long shift = size / nthreads;
    for (int i = 0; i < nthreads; ++i) {
        args[i].data = data;
        args[i].beg = i * shift;
        args[i].end = (i < nthreads - 1) ? args[i].beg + shift : size;
        args[i].value = 1111;
        args[i].index = -1;
        pthread_create(&threads[i], NULL, simple_search, &args[i]);
    }

    long ret = -1;
    for (int i = 0; i < nthreads; i++) {
        pthread_join(threads[i], NULL);
        if (args[i].index != -1)
            ret = args[i].index;
    }

    free(args);
    free(threads);

    return ret;
}

int main(int argc, char** argv) {

    long size = 100 * 1000 * 1000;
    if (argc > 1) {
        char *end;
        long n = strtol(argv[1], &end, 10);
        if (n != 0) size = n;
    }
    setbuf(stdout, NULL);

    printf("Generate test data of %ld integers... ", size);
    int* data = (int*)malloc(size * sizeof(int));
    for (int i = 0; i < size; i++)
        data[i] = (int)(random() % 1000);
    long index = size - size / 10;
    data[index] = 1111;
    printf("Done, set data[%ld] = %d\n", index, data[index]);

    printf(" 1-thread int search started...");
    struct timeval tbefore, tafter, telapsed;
    gettimeofday(&tbefore, NULL);
    long si = int_search(data, size, 1);
    gettimeofday(&tafter, NULL);
    timersub(&tafter, &tbefore, &telapsed);
    printf(" Elapsed: %ld.%06ld\n", (long int)telapsed.tv_sec, (long int)telapsed.tv_usec);
    printf("Founded index of magic number is %ld, %s\n", si, si == index ? "OK" : "ERROR");

    int nthreads = get_nprocs();
    if (nthreads > size) nthreads = (int)size;
    printf("%2d-thread int search started...", nthreads);
    gettimeofday(&tbefore, NULL);
    si = int_search(data, size, nthreads);
    gettimeofday(&tafter, NULL);
    timersub(&tafter, &tbefore, &telapsed);
    printf(" Elapsed: %ld.%06ld\n", (long int)telapsed.tv_sec, (long int)telapsed.tv_usec);
    printf("Founded index of magic number is %ld, %s\n", si, si == index ? "OK" : "ERROR");

    free(data);

    return 0;
}
