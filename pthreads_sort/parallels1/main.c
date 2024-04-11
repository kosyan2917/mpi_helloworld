#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <sys/sysinfo.h>
#include <pthread.h>
#include <sys/time.h>

int cmp_int_desc(const void* a, const void* b) {
    return *(int*)b - *(int*)a;
}

typedef struct {
    int* data;
    long size;
} SortArgs;

void* simple_sort(void* args) {
    SortArgs *a = (SortArgs*)args;
    qsort(a->data, a->size, sizeof a->data[0], cmp_int_desc);
    return NULL;
}

void merge(int* data, long size, int nthreads) {
    typedef struct {
        int* data;
        long size;
    } MergeData;
    MergeData* md = (MergeData*)malloc(nthreads * sizeof(*md));
    for (int i = 0; i < nthreads; ++i) {
        md[i].data = data + i * size / nthreads;
        md[i].size = size / nthreads;
    }
    int *tmp = (int*)malloc(size * sizeof(int));
    int *out = tmp;

    while (1) {
        long midx = -1;
        for (int i = 0; i < nthreads; ++i) {
            if (md[i].size > 0) {
                if (midx < 0 || md[i].data[0] > md[midx].data[0])
                    midx = i;
            }
        }
        if (midx < 0) break;

        *out++ = *md[midx].data++;
        md[midx].size--;
    }

    memcpy(data, tmp, size * sizeof(int));
    free(tmp);
}

void msort(int* data, long size, int nthreads) {
    pthread_t *threads = (pthread_t*)malloc(nthreads * sizeof(*threads));
    SortArgs *args = (SortArgs*)malloc(nthreads * sizeof(*args));

    long csize = size / nthreads;
    for (int i = 0; i < nthreads; ++i) {
        args[i].data = data + i * csize;
        args[i].size = csize;
        pthread_create(&threads[i], NULL, simple_sort, &args[i]);
    }

    for (int i = 0; i < nthreads; i++)
        pthread_join(threads[i], NULL);

    merge(data, size, nthreads);

    free(args);
    free(threads);
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
    int nthreads = get_nprocs();
    if (nthreads > size) nthreads = (int)size;
    long nsize = (size + nthreads - 1) / nthreads * nthreads;    // adjust input size for simplify merge sorting
    int* data0 = (int*)malloc(nsize * sizeof(int));
    int* data1 = (int*)malloc(nsize * sizeof(int));
    for (long i = 0; i < size; i++)
        data0[i] = data1[i] = (int)(random());
    for (long i = size; i < nsize; i++)
        data0[i] = data1[i] = 0;
    puts("Done");

    printf(" 1-thread qsort started...");
    struct timeval tbefore, tafter, telapsed;
    gettimeofday(&tbefore, NULL);
    qsort(data0, size, sizeof data0[0], cmp_int_desc);
    gettimeofday(&tafter, NULL);
    timersub(&tafter, &tbefore, &telapsed);
    printf(" Elapsed: %ld.%06ld\n", (long int)telapsed.tv_sec, (long int)telapsed.tv_usec);

    printf("%2d-thread msort started...", nthreads);
    gettimeofday(&tbefore, NULL);
    msort(data1, nsize, nthreads);
    gettimeofday(&tafter, NULL);
    timersub(&tafter, &tbefore, &telapsed);
    printf(" Elapsed: %ld.%06ld\n", (long int)telapsed.tv_sec, (long int)telapsed.tv_usec);

    printf("Check the results... ");
    int res = memcmp(data0, data1, size * sizeof(int));
    puts(res ? "Failed" : "Passed");

    free(data0);
    free(data1);

    return 0;
}
