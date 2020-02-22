# Multi-Threading

## Table of Contents
1. [Threads](#section1)
2. [Spinlocks](#section2)
3. [Mutexes](#section3)
4. [Deadlocks](#section4)

C language programs: &nbsp; [C1](#c1) &nbsp; [C2](#c2) &nbsp; [C3](#c3) &nbsp; [C4](#c4)

## Threads <a name="section1"></a>

**C 1**: <a name="c1"></a>The program [multithreads.c](multithreads.c) runs three concurrent instances of the `my_thread()` loop function by creating three [POSIX threads][POSIX_THREADS] where each instance accesses a global variable `g`, increments it by one, sums `g` 10000 times with itself and prints out this sum. In order to ease the load on the processor cores a short  sleep period of 1 ms is added to each loop.

```C
 1 #include <stdio.h>
 2 #include <stdlib.h>
 3 #include <stdint.h>
 4 #include <unistd.h>
 5 #include <pthread.h>
 6
 7 /* number of threads */
 8 #define N	3
 9
10 /* global variable incremented by threads */
11 uint32_t g = 0;
12
13 /* function to be executed by all threads */
14 void *my_thread(void *vargp)
15 {
16     uint32_t my_id = (uintptr_t)vargp;
17     uint32_t iterations = 10000;
18
19     while (iterations--)
20     {
21         uint32_t rounds = 10000, sum = 0, inc;
22
23         /* Increment global variable */
24         inc = ++g;
25
26         while (rounds--)
27         {
28             sum += inc;
29         }
30
31         /* sleep 1 millisecond */
32         usleep(1000);
33
34         printf("Thread %u: inc = %5u, sum = %9u\n", my_id, inc, sum);
35     }
36 }
37
38 int main()
39 {
40     pthread_t tid[N];
41     uint32_t i, id;
42
43     /* create N threads */
44     for (i = 0; i < N; i++)
45     {
46         id = i + 2;
47         pthread_create(&tid[i], NULL, my_thread, (void*)(uintptr_t)id);
48         printf("Started thread %d: 0x%012" PRIx64 "\n", id, (uint64_t)tid[i]);
49     }
50
51     /* wait for threads to terminate */
52     for (i = 0; i < N; i++)
53     {
54         pthread_join(tid[i], NULL);
55     }
56     exit(0);
57 }
```
We compile `multithreads.c` with
```console
> gcc -ggdb -o multithreads multithreads.c -lpthread
```
and then start it on the command line
```console
> ./multithreads
Started thread 2: 0x7fe3a2a9c700
Started thread 3: 0x7fe3a229b700
Started thread 4: 0x7fe3a1a9a700
Thread 2: inc =     1, sum =     10000
Thread 4: inc =     2, sum =     20000
Thread 3: inc =     3, sum =     30000
Thread 2: inc =     4, sum =     40000
Thread 4: inc =     5, sum =     50000
Thread 3: inc =     6, sum =     60000
...
Thread 3: inc = 29994, sum = 299940000
Thread 4: inc = 29995, sum = 299950000
Thread 3: inc = 29996, sum = 299960000
Thread 3: inc = 29997, sum = 299970000
Thread 3: inc = 29998, sum = 299980000
Thread 3: inc = 29999, sum = 299990000
Thread 3: inc = 30000, sum = 300000000
```
The four CPUs of the processor get quite busy since the individual threads are spread evenly over all cores 

![CPU Load with Multithreading][THREADS_IMG]

Each thread terminates after 10'000 iterations taking about 15 seconds to complete. 10 seconds are spent in the `usleep` statement and some additional seconds are needed for the 100'000'000 additions per thread. 

in a next step we want to have closer look at the threads with the `gdb` debugger

```console
> gdb multithreads
(gdb) break 34
Breakpoint 1 at 0x80b: file multithreads.c, line 34.
(gdb) run
Starting program: /home/andi/Documents/Cyber RS/cyber/Operating_Systems/multithreads
[Thread debugging using libthread_db enabled]
Using host libthread_db library "/lib/x86_64-linux-gnu/libthread_db.so.1".
[New Thread 0x7ffff77c4700 (LWP 6016)]
Started thread 2: 0x7ffff77c4700
[New Thread 0x7ffff6fc3700 (LWP 6017)]
Started thread 3: 0x7ffff6fc3700
[New Thread 0x7ffff67c2700 (LWP 6018)]
Started thread 4: 0x7ffff67c2700
[Switching to Thread 0x7ffff77c4700 (LWP 6016)]
Thread 2 "multithreads" hit Breakpoint 1, my_thread (vargp=0x2) at multithreads.c:34
34	        printf("Thread %u: inc = %5u, sum = %9u\n", my_id, inc, sum);
```
We are now shortly before the `printf()` statement in thread `2`. Let's have a look at the local variables:
```console
(gdb) info locals
rounds = 4294967295
sum = 10000
inc = 1
my_id = 2
iterations = 9999
```
We continue the program flow and end up at the breakpoint of thread `3`
```console
(gdb) continue
Continuing.
[Switching to Thread 0x7ffff6fc3700 (LWP 6017)]
Thread 3 "multithreads" hit Breakpoint 1, my_thread (vargp=0x3) at multithreads.c:34
34	        printf("Thread %u: inc = %5u, sum = %9u\n", my_id, inc, sum);
```
Thread `3` has its own set of local variables on the stack:
```console
(gdb) info locals
rounds = 4294967295
sum = 20000
inc = 2
my_id = 3
iterations = 9999
```
We continue the program flow and end up at the breakpoint of thread `4`
```console
(gdb) continue
Continuing.
[Switching to Thread 0x7ffff67c2700 (LWP 6018)]
Thread 4 "multithreads" hit Breakpoint 1, my_thread (vargp=0x4) at multithreads.c:34
34	        printf("Thread %u: inc = %5u, sum = %9u\n", my_id, inc, sum);
```
Again we  check the local variables
```console
(gdb) info locals
rounds = 4294967295
sum = 30000
inc = 3
my_id = 4
iterations = 9999
```
The command `info threads` gives an overview over all existing threads and shows that we are currently monitoring thread `4`
```console
(gdb) info threads
  Id   Target Id         Frame
  1    Thread 0x7ffff7fd2740 (LWP 6012) "multithreads" 0x00007ffff7bbed2d in __GI___pthread_timedjoin_ex (threadid=140737345505024, thread_return=0x0, abstime=0x0, block=<optimized out>) at pthread_join_common.c:89
  2    Thread 0x7ffff77c4700 (LWP 6016) "multithreads" 0x000055555555480e in my_thread (vargp=0x2) at multithreads.c:34
  3    Thread 0x7ffff6fc3700 (LWP 6017) "multithreads" my_thread (vargp=0x3) at multithreads.c:34
* 4    Thread 0x7ffff67c2700 (LWP 6018) "multithreads" my_thread (vargp=0x4) at multithreads.c:34
```
Thread `1` is the main thread waiting for the other threads to terminate.

We continue in the program flow, get the print output of all three threads after the `usleep `  statement and stop again at thread `2`
```console
(gdb) continue
Continuing.
Thread 2: inc =     1, sum =     10000
Thread 3: inc =     2, sum =     20000
[Switching to Thread 0x7ffff77c4700 (LWP 6016)]
Thread 2 "multithreads" hit Breakpoint 1, my_thread (vargp=0x2) at multithreads.c:34
34	        printf("Thread %u: inc = %5u, sum = %9u\n", my_id, inc, sum);
(gdb) info locals
rounds = 4294967295
sum = 40000
inc = 4
my_id = 2
iterations = 9998
```
After two iterations the `inc` variable in thread `2` has taken on the value `4` and we can determine its location on the stack:
```console
gdb) print/x &inc
$1 = 0x7ffff77c3eec
```
We want to check the stack location of `inc` in thread `4` and switch to it
```console
gdb) thread 4
[Switching to thread 4 (Thread 0x7ffff67c2700 (LWP 6018))]
#0  0x000055555555480e in my_thread (vargp=0x4) at multithreads.c:34
34	        printf("Thread %u: inc = %5u, sum = %9u\n", my_id, inc, sum);
(gdb) print/x &inc
$2 = 0x7ffff67c1eec
```
Next we switch to thread 3 which is still stuck in the sleep phase
```console
gdb) thread 3
[Switching to thread 3 (Thread 0x7ffff6fc3700 (LWP 6017))]
#0  0x00007ffff78a99d0 in __GI___nanosleep (requested_time=requested_time@entry=0x7ffff6fc2e90, remaining=remaining@entry=0x0)  at ../sysdeps/unix/sysv/linux/nanosleep.c:28
28	../sysdeps/unix/sysv/linux/nanosleep.c: No such file or directory.
```
so we advance the program flow
```console
(gdb) continue
Continuing.
Thread 4: inc =     3, sum =     30000
Thread 3 "multithreads" hit Breakpoint 1, my_thread (vargp=0x3) at multithreads.c:34
34	        printf("Thread %u: inc = %5u, sum = %9u\n", my_id, inc, sum);
(gdb) print/x &inc
$3 = 0x7ffff6fc2eec
```
We notice that each thread has its own stack area.
| Thread | Stack location of `inc` variable |
| :----: | :------------------------------- |
|   2    | `0x7ffff77c3eec`                 |
|   3    | `0x7ffff6fc2eec`                 |
|   4    | `0x7ffff67c1eec `                |

[POSIX_THREADS]: https://en.wikipedia.org/wiki/POSIX_Threads
[THREADS_IMG]: Multithreads_500.png

## Spinlocks <a name="section2"></a>

If we execute the `multithreads` program several times, it sometimes happens that the final value of the `inc` variable in the last thread to rejoin the main program is not `30000` but `299999` or even `29998` as shown in the run below:
```console
> ./multithreads
...
Thread 2: inc = 29993, sum = 299930000
Thread 3: inc = 29994, sum = 299940000
Thread 3: inc = 29996, sum = 299960000
Thread 2: inc = 29995, sum = 299950000
Thread 2: inc = 29997, sum = 299970000
Thread 2: inc = 29998, sum = 299980000
```
The problem is that multiple threads try to access and increment the global variable `g` simultaneously in line `24` of `my_thread()` with the result that two threads might get the same value of `g`:
```C
23        /* Increment global variable */
24        inc = ++g;
```
In order to prevent this from happening we need a lock which makes the fetch and increment operation `inc = ++g` *atomic*.

**C 2**: <a name="c2"></a>One solution is to use a *POSIX Spinlock* which is applied to the atomic fetch and update operation as implemented in [multithreads_spinlock.c](multithreads_spinlock.c):

```C
 1 #include <stdio.h>
 2 #include <stdlib.h>
 3 #include <stdint.h>
 4 #include <unistd.h>
 5 #include <pthread.h>
 6
 7 /* number of threads */
 8 #define N	3
 9
10 /* global variable incremented by threads */
11 uint32_t g = 0;
12
13 /* global spinlock protecting g */
14 pthread_spinlock_t spinlock;
15
16 /* function to be executed by all threads */
17 void *my_thread(void *vargp)
18 {
19     uint32_t my_id = (uintptr_t)vargp;
20     uint32_t iterations = 10000;
21
22     while (iterations--)
23     {
24         uint32_t rounds = 10000, sum = 0, inc;
25
26         /* Increment global variable g with an atomic operation */
27         pthread_spin_lock(&spinlock);
28         inc = ++g;
29         pthread_spin_unlock(&spinlock);
30
31         while (rounds--)
32         {
33             sum += inc;
34         }
35
36         /* sleep 1 millisecond */
37         usleep(1000);
38
39         printf("Thread %u: inc = %5u, sum = %9u\n", my_id, inc, sum);
40     }
41 }
42
43 int main()
44 {
45     pthread_t tid[N];
46     uint32_t i, id;
47
48     /* init spinlock */
49     pthread_spin_init(&spinlock, PTHREAD_PROCESS_PRIVATE);
50
51     /* create N threads */
52     for (i = 0; i < N; i++)
53     {
54         id = i + 2;
55         pthread_create(&tid[i], NULL, my_thread, (void*)(uintptr_t)id);
56         printf("Started thread %d: %p\n", id, (void *)tid[i]);
57     }
58
59     /* wait for threads to terminate */
60     for (i = 0; i < N; i++)
61     {
62         pthread_join(tid[i], NULL);
63     }
64
65     /* deinit spinlock */
66     pthread_spin_destroy(&spinlock);
67
68     exit(0);
69 }
```
Because blocked threads are put into a busy-waiting-loop, *POSIX Spinlocks* are ideal if the required locking is only for a very short time as is the case in updating a shared global variable.
```console
> gcc -ggdb -o multithreads_spinlock multithreads_spinlock.c -lpthread
> ./multithreads_spinlocks
Started thread 2: 0x7ff2a53c5700
Started thread 3: 0x7ff2a4bc4700
Started thread 4: 0x7ff2a43c3700
Thread 2: inc =     1, sum =     10000
Thread 4: inc =     2, sum =     20000
Thread 3: inc =     3, sum =     30000
Thread 2: inc =     4, sum =     40000
Thread 4: inc =     5, sum =     50000
Thread 2: inc =     6, sum =     60000
Thread 3: inc =     7, sum =     70000
...
Thread 3: inc = 29995, sum = 299950000
Thread 4: inc = 29996, sum = 299960000
Thread 2: inc = 29997, sum = 299970000
Thread 2: inc = 29998, sum = 299980000
Thread 2: inc = 29999, sum = 299990000
Thread 2: inc = 30000, sum = 300000000
```
With the *POSIX Spinlock* in place exactly `30000` increments are done in each run.

## Mutexes <a name="section3"></a>

If a thread has to lock a longer sequence of instructions e.g. an  insertion or deletion of an entry into a linked list then *POSIX Mutexes* are more suitable than *POSIX Spinlocks* because the waiting threads are put to sleep until the the mutex becomes unlocked.

**C 3**: <a name="c3"></a>Use of a *POSIX Mutex* applied to the atomic fetch and update operation as implemented in [multithreads_mutex.c](multithreads_mutex.c):

```C
 1 #include <stdio.h>
 2 #include <stdlib.h>
 3 #include <stdint.h>
 4 #include <unistd.h>
 5 #include <pthread.h>
 6
 7 /* number of threads */
 8 #define N	3
 9 
10 /* global variable incremented by threads */
11 uint32_t g = 0;
12
13 /* global mutex protecting g */
14 pthread_mutex_t mutex;
15
16 /* function to be executed by all threads */
17 void *my_thread(void *vargp)
18 {
19     uint32_t my_id = (uintptr_t)vargp;
20     uint32_t iterations = 10000;
21
22     while (iterations--)
23    {
24         uint32_t rounds = 10000, sum = 0, inc;
25
26         /* Increment global variable g with an atomic operation */
27         pthread_mutex_lock(&mutex);
28         inc = ++g;
29         /* sleep 0.1 milliseconds */
30         usleep(100);
31         pthread_mutex_unlock(&mutex);
32
33         while (rounds--)
34         {
35             sum += inc;
36         }
37
38         /* sleep 0.9 milliseconds */
39         usleep(900);
40
41         printf("Thread %u: inc = %5u, sum = %9u\n", my_id, inc, sum);
42     }
43 }
44
45 int main()
46 {
47     pthread_t tid[N];
48     uint32_t i, id;
49
50     /* init mutex */
51     pthread_mutex_init(&mutex, NULL);
52
53     /* create N threads */
54     for (i = 0; i < N; i++)
55     {
56         id = i + 2;
57         pthread_create(&tid[i], NULL, my_thread, (void*)(uintptr_t)id);
58         printf("Started thread %d: %p\n", id, (void *)tid[i]);
59     }
60
61     /* wait for threads to terminate */
62     for (i = 0; i < N; i++)
63     {
64         pthread_join(tid[i], NULL);
65     }
66
67     /* deinit mutex */
68     pthread_mutex_destroy(&mutex);
69
70     exit(0);
71 }
```
A trial run:
```console
> gcc -ggdb -o multithreads_mutex multithreads_mutex.c -lpthread
> ./multithreads_mutex
Started thread 2: 0x7f21c00e9700
Started thread 3: 0x7f21bf8e8700
Started thread 4: 0x7f21bf0e7700
Thread 3: inc =     1, sum =     10000
Thread 2: inc =     2, sum =     20000
Thread 4: inc =     3, sum =     30000
Thread 3: inc =     4, sum =     40000
Thread 2: inc =     5, sum =     50000
Thread 4: inc =     6, sum =     60000
...
Thread 3: inc = 29996, sum = 299960000
Thread 2: inc = 29995, sum = 299950000
Thread 3: inc = 29997, sum = 299970000
Thread 3: inc = 29998, sum = 299980000
Thread 3: inc = 29999, sum = 299990000
Thread 3: inc = 30000, sum = 300000000
```
Again exactly `30000`increments are executed.

## Deadlocks <a name="section4"></a>

**C 4**: <a name="c4"></a>Wrong use of multiple *POSIX Mutexes* can lead to *Deadlocks* as shown in [multithreads_deadlock.c](multithreads_deadlock.c):

```C
 1 #include <stdio.h>
 2 #include <stdlib.h>
 3 #include <stdint.h>
 4 #include <unistd.h>
 5 #include <pthread.h>
 6
 7 /* number of threads */
 8 #define N	2
 9
10 /* global variables incremented by threads */
11 uint32_t g1 = 0, g2 = 0;
12
13 /* global mutexes protecting g1 and g2, respectively */
14 pthread_mutex_t mutex1, mutex2;
15
16 /* function to be executed by all threads */
17 void *my_thread(void *vargp)
18 {
19     uint32_t my_id = (uintptr_t)vargp;
20     uint32_t iterations = 10000;
21
22     while (iterations--)
23     {
24         uint32_t rounds = 10000, g;
25
26         if (my_id == 1)
27         {
28             /* thread 1 only */
29             pthread_mutex_lock(&mutex1);
30             g = g1;
31             /* sleep 0.1 milliseconds */
32             usleep(100);
33
34             pthread_mutex_lock(&mutex2);
35             g2 = ++g;
36             pthread_mutex_unlock(&mutex2);
37           
38             pthread_mutex_unlock(&mutex1);
39         }
40         else
41         {
42             /* thread 2 only */
43             pthread_mutex_lock(&mutex2);
44             g = g2;
45             /* sleep 0.1 milliseconds */
46             usleep(100);
47
48             pthread_mutex_lock(&mutex1);
49             g1 = ++g;
50             pthread_mutex_unlock(&mutex1);
51            
52             pthread_mutex_unlock(&mutex2);
53         }
54
55         /* sleep 0.9 milliseconds */
56         usleep(900);
57
58         printf("Thread %u: g = %5u\n", my_id, g);
59     }
60 }
61
62 int main()
63 {
64     pthread_t tid[N];
65     uint32_t i, id;
66
67     /* init mutexes */
68     pthread_mutex_init(&mutex1, NULL);
69     pthread_mutex_init(&mutex2, NULL);
70
71     /* create N threads */
72     for (i = 0; i < N; i++)
73     {
74         id = i + 1;
75         pthread_create(&tid[i], NULL, my_thread, (void*)(uintptr_t)id);
76         printf("Started thread %d: %p\n", id, (void *)tid[i]);
77     }
78
79     /* wait for threads to terminate */
80     for (i = 0; i < N; i++)
81     {
82         pthread_join(tid[i], NULL);
83     }
84
85     /* deinit mutexes */
86     pthread_mutex_destroy(&mutex1);
87     pthread_mutex_destroy(&mutex2);
88
89     exit(0);
90 }
```
We compile and start the program:

```console
> gcc -ggdb -o multithreads_deadlock multithreads_deadlock.c -lpthread
> ./multithreads_deadlock 
Started thread 1: 0x7f401e45b700
Started thread 2: 0x7f401dc5a700
Thread 1: g =     1
Thread 2: g =     2
Thread 1: g =     3
Thread 2: g =     4
Thread 1: g =     5
Thread 2: g =     6
```
In this run after three iterations a deadlock occurs. Trying multiple times we see that the actual time until the two threads lock varies:
```console
> ./multithreads_deadlock 
Started thread 1: 0x7f72834d3700
Started thread 2: 0x7f7282cd2700
Thread 1: g =     1
Thread 2: g =     2
```

Author:  [Andreas Steffen][AS] [CC BY 4.0][CC]

[AS]: mailto:andreas.steffen@strongsec.net
[CC]: http://creativecommons.org/licenses/by/4.0/
