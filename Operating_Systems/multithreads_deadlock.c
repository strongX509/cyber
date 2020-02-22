#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <pthread.h>

/* number of threads */
#define N	2

/* global variables incremented by threads */
uint32_t g1 = 0, g2 = 0;

/* global mutexes protecting g1 and g2, respectively */
pthread_mutex_t mutex1, mutex2;

/* function to be executed by all threads */
void *my_thread(void *vargp)
{
    uint32_t my_id = (uintptr_t)vargp;
    uint32_t iterations = 10000;

    while (iterations--)
    {
        uint32_t rounds = 10000, g;

        if (my_id == 1)
        {
            /* thread 1 only */
            pthread_mutex_lock(&mutex1);
            g = g1;
            /* sleep 0.1 milliseconds */
            usleep(100);

            pthread_mutex_lock(&mutex2);
            g2 = ++g;
            pthread_mutex_unlock(&mutex2);
           
            pthread_mutex_unlock(&mutex1);
        }
        else
        {
            /* thread 2 only */
            pthread_mutex_lock(&mutex2);
            g = g2;
            /* sleep 0.1 milliseconds */
            usleep(100);

            pthread_mutex_lock(&mutex1);
            g1 = ++g;
           pthread_mutex_unlock(&mutex1);
            
            pthread_mutex_unlock(&mutex2);
        }

        /* sleep 0.9 milliseconds */
        usleep(900);

        printf("Thread %u: g = %5u\n", my_id, g);
    }
}

int main()
{
    pthread_t tid[N];
    uint32_t i, id;

    /* init mutexes */
    pthread_mutex_init(&mutex1, NULL);
    pthread_mutex_init(&mutex2, NULL);

    /* create N threads */
    for (i = 0; i < N; i++)
    {
        id = i + 1;
        pthread_create(&tid[i], NULL, my_thread, (void*)(uintptr_t)id);
        printf("Started thread %d: %p\n", id, (void *)tid[i]);
    }

    /* wait for threads to terminate */
    for (i = 0; i < N; i++)
    {
        pthread_join(tid[i], NULL);
    }

    /* deinit mutex */
    pthread_mutex_destroy(&mutex1);
    pthread_mutex_destroy(&mutex2);

    exit(0);
}
