#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/shm.h>
#include <sys/wait.h>

/* number of child processes */
#define N	3

typedef struct {
    /* variable incremented jointly by child processes */
    uint32_t g;
    /* mutex protecting variable g */
    pthread_mutex_t mutex;
} shared_t;

/* pointer to shared_t object */
shared_t *shared;

/* shared memory segment ID */
int shm_id;

int main()
{
    pid_t pid;
    int id;

    printf("Parent : pid %d\n", (int)getpid());

    /* parent process creates the shared memory segment */
    shm_id = shmget(IPC_PRIVATE, sizeof(shared_t), IPC_CREAT | 0666);
    if (shm_id < 0)
    {
         exit(-1);
    }
    printf("Parent : allocated %d bytes at segment %d\n",
            (int)sizeof(shared_t), shm_id);

    /* parent process attaches the shared segment */
    shared = (shared_t *) shmat(shm_id, NULL, 0);

    if (shared == (shared_t *) -1)
    {
        printf("Parent : attaching to segment %d failed\n", shm_id);
        shmctl(shm_id, IPC_RMID, NULL);
        exit(-1);
    }
    printf("Parent : attached to segment %d\n", shm_id);
    fflush(stdout);

    /* parent process initializes mutex so it works properly in shared memory */
    {
        pthread_mutexattr_t attr;

        pthread_mutexattr_init(&attr);
        pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
        pthread_mutex_init(&shared->mutex, &attr);
    }

    /* parent process initalizes shared variable g */
    shared->g = 0;

    /* parent process forks the child processes */
    for (id = 1; id <= N; id++)
    {
        pid = fork();

        /* the fork() command returns pid = 0 in child processes */
        if (pid > 0)
        {
            printf("Parent : forked child %d with pid %d\n", id, (int)pid);
            fflush(stdout);
          }
        else
        {
            break;
        }
    }

    if (pid == 0)
    {
        uint32_t iterations = 10000;

        /* each child process attaches the shared segment */
        shared = (shared_t *) shmat(shm_id, NULL, 0);

        if (shared == (shared_t *) -1)
        {
            printf("Child %d: attaching to segment failed\n", id);
        }
        else
        {
            printf("Child %d: attached to segment %d\n", id, shm_id);
        }

        /* each child process uses the shared segment */
        while (iterations--)
        {
            uint32_t rounds = 10000, sum = 0, inc;

            /* increment global variable g with an atomic operation */
            pthread_mutex_lock(&shared->mutex);
            inc = ++shared->g;
            pthread_mutex_unlock(&shared->mutex);

            while (rounds--)
            {
                sum += inc;
            }

            /* sleep 1 millisecond */
            usleep(1000);

            printf("Child %d: inc = %5u, sum = %9u\n", id, inc, sum);
            fflush(stdout);
        }

        /* each child process detaches the segment */
        if (shmdt(shared) == -1)
        {
            printf("Child %d: detaching segment %d failed\n", id, shm_id);
        }
        else
        {
            printf("Child %d: detached segment %d\n", id, shm_id);
        }
    }
    else
    {
        /* parent process waits for child processes to exit */
        for (id = 1; id <= N; id++)
        {
            wait(NULL);
        }
 
        /* parent process destroys the mutex */
        pthread_mutex_destroy(&shared->mutex);

        /* parent process detaches the segment */
        if (shmdt(shared) == -1)
        {
            printf("Parent : detaching segment %d failed\n", shm_id);
        }
        else
        {
            printf("Parent : detached segment %d\n", shm_id);
        }

        /* parent process deletes the segment */
        if (shmctl(shm_id, IPC_RMID, NULL) == -1)
        {
            printf("Parent : deleting segment %d failed\n", shm_id);
        }
        else
        {
            printf("Parent : deleted segment %d\n", shm_id);
        }
    }

    exit(0);
}
