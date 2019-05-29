#include <u.h>
#include <libc.h>
#include <thread.h>

/* Resources:
 * [1] Plan 9 manual: thread(3).
 * [2] BALLESTEROS, Francisco J. Introduction to Operating Systems Abstractions
 *     Using Plan 9 From Bell Labs. 2007.
 */


Channel* chn;


void
sleepsort_thr(void *arg)
{
    int *num = arg;
    vlong target_time;

    /* Sleeping does not give back control
     * to another context. Calc target time and
     * yield as needed instead.
     * Miliseconds seem to work best. */
    
    target_time = nsec() + (1000000 * (*num));

    while(nsec() < target_time) {
        yield();
    }

    send(chn, &num);
    threadexits(nil);
}

void
sleepsort(int *numbers, int size)
{
    int i;
    chn = chancreate(sizeof(int*), size);

    // Dispatch coroutines
    for(i = 0; i < size; i++) {
        threadcreate(sleepsort_thr, (numbers + i), 8 * 1024);
    }

    // Await results, print them one by one
    for(i = 0; i < size; i++) {
        int *num;
        recv(chn, &num);
        print("%d\n", *num);
    }
    
    chanfree(chn);
}

void
threadmain(int argc, char **argv)
{
    if(argc <= 1) {
        print("%s [numbers...]\n", argv[0]);
    }

    int i, *numbers;
    numbers = malloc((argc - 1) * sizeof(int));

    // Into int vector
    for(i = 0; i < argc - 1; i++) {
        numbers[i] = atoi(argv[i + 1]);
    }

    sleepsort(numbers, argc - 1);


    free(numbers);
    threadexits(nil);
}
