#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

pthread_mutex_t mutex;

// Queue

typedef struct node_s {
    struct node_s* next;
    int value;
} node_t;

typedef struct {
    node_t* head;
    node_t* tail;
} queue_t;

void queue_init(queue_t** queue) {
    *queue = (queue_t*)malloc(sizeof(queue_t));
    node_t* dummy = (node_t*)malloc(sizeof(node_t));
    dummy->value = -1;
    dummy->next = NULL;

    (*queue)->head = dummy;
    (*queue)->tail = dummy;
}

void queue_push(queue_t* queue, int value) {
    node_t* last;
    node_t* next;

    node_t* node = (node_t*)malloc(sizeof(node_t));
    node->value = value;
    node->next = NULL;

    while (1) {
        last = queue->tail;
        next = last->next;

        if (last == queue->tail) {
            if (next == NULL) {
                if (__sync_bool_compare_and_swap(&last->next, next, node)) {
                    __sync_bool_compare_and_swap(&queue->tail, last, node);
                    return;
                }
            } else {
                __sync_bool_compare_and_swap(&queue->tail, last, next);
            }
        }
    }
}

void queue_pull(queue_t* queue) {
    node_t* head;
    node_t* new_head;
    node_t* tail;

    while (1) {
        head = queue->head;
        tail = queue->tail;
        new_head = head->next;

        if (head == queue->head) {
            if (head == tail) {
                if (new_head == NULL) {
                    return;
                }
                __sync_bool_compare_and_swap(&queue->tail, tail, new_head);
            } else {
                if (__sync_bool_compare_and_swap(&queue->head, head, new_head)) {
                    break;
                }
            }
        }
    }
    free(head);
    return;

}

void queue_pull_simple(queue_t* queue) {
    pthread_mutex_lock(&mutex);
    node_t* head = queue->head;
    node_t* new_head = head->next;
    if (new_head == NULL) {
        pthread_mutex_unlock(&mutex);
        return;
    }
    queue->head = new_head;
    pthread_mutex_unlock(&mutex);
    free(head);

}

void queue_push_simple(queue_t* queue, int value) {
    node_t* node = (node_t*)malloc(sizeof(node_t));
    node->value = value;
    node->next = NULL;

    pthread_mutex_lock(&mutex);
    queue->tail->next = node;
    queue->tail = node;
    pthread_mutex_unlock(&mutex);
}

queue_t* queue;

// Main code

volatile int sum = 0;

void* atomic_thr(void* args) {
    for (int i = 0; i < 10000000; ++i) {
        queue_push(queue, i);
//        queue_push_simple(queue, i);
    }
    for (int i = 0; i < 10000000; ++i) {
        queue_pull(queue);
//        queue_pull_simple(queue);
    }
}

void* mutex_thr(void* args) {
    for (int i = 0; i < 10000000; ++i) {
        queue_push_simple(queue, i);
    }
    for (int i = 0; i < 10000000; ++i) {
        queue_pull_simple(queue);
    }
}

void* atomic_test() {
    pthread_t thread0;
    pthread_t thread1;
    pthread_create(&thread0, nullptr, atomic_thr, nullptr);
    pthread_create(&thread1, nullptr, atomic_thr, nullptr);
    pthread_join(thread0, NULL);
    pthread_join(thread1, NULL);
}

void* mutex_test() {
    pthread_t thread0;
    pthread_t thread1;
    pthread_create(&thread0, nullptr, mutex_thr, nullptr);
    pthread_create(&thread1, nullptr, mutex_thr, nullptr);
    pthread_join(thread0, NULL);
    pthread_join(thread1, NULL);
}


int main() {
    pthread_mutex_init(&mutex, NULL);

    queue_init(&queue);
    setbuf(stdout, NULL);
    printf("atomic test started");
    struct timeval tbefore, tafter, telapsed;
    gettimeofday(&tbefore, NULL);
    atomic_test();
    gettimeofday(&tafter, NULL);
    timersub(&tafter, &tbefore, &telapsed);
    printf(" Elapsed: %ld.%06ld\n", (long int)telapsed.tv_sec, (long int)telapsed.tv_usec);

    printf("mutex test started");
    gettimeofday(&tbefore, NULL);
    mutex_test();
    gettimeofday(&tafter, NULL);
    timersub(&tafter, &tbefore, &telapsed);
    printf(" Elapsed: %ld.%06ld\n", (long int)telapsed.tv_sec, (long int)telapsed.tv_usec);


    return 0;
}
