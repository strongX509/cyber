#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <pthread.h>

/* number of threads */
#define N	3

/* global variable incremented by threads */
uint32_t g = 0;

/* global spinlock protecting g */
pthread_spinlock_t spinlock;

/* function to be executed by all threads */
void *my_thread(void *vargp)
{
    uint32_t my_id = (uintptr_t)vargp;
    uint32_t iterations = 10000;

    while (iterations--)
    {
        uint32_t rounds = 10000, sum = 0, inc;

        /* Increment global variable g with an atomic operation */
        pthread_spin_lock(&spinlock);
        inc = ++g;
        pthread_spin_unlock(&spinlock);

        while (rounds--)
        {
            sum += inc;
        }

        /* sleep 1 millisecond */
        usleep(1000);

        printf("Thread %u: inc = %5u, sum = %9u\n", my_id, inc, sum);
    }
}

int main()
{
    pthread_t tid[N];
    uint32_t i, id;

    /* init spinlock */
    pthread_spin_init(&spinlock, PTHREAD_PROCESS_PRIVATE);

    /* create N threads */
    for (i = 0; i < N; i++)
    {
        id = i + 2;
        pthread_create(&tid[i], NULL, my_thread, (void*)(uintptr_t)id);
        printf("Started thread %d: %p\n", id, (void *)tid[i]);
    }

    /* wait for threads to terminate */
    for (i = 0; i < N; i++)
    {
        pthread_join(tid[i], NULL);
    }

    /* deinit spinlock */
    pthread_spin_destroy(&spinlock);

    exit(0);
}
