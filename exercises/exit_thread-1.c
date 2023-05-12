/*This program should fetch the exit code from threads tid1 and tid2.
  Expected Output:
  thread 1 returning
  thread 2 exiting
  thread 1 exit code 1
  thread 2 exit code 2
 
  Fix the code to produce the expected output.
 */
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

void *thr_fn1(void *arg)
{
    printf("thread 1 returning\n");
    return((void *)1);
}

void *thr_fn2(void *arg)
{
    printf("thread 2 exiting\n");
    pthread_exit((void *)2);
}

int main()
{
    int          err;
    pthread_t    tid1, tid2;
    void         *tret=0;

    err = pthread_create(&tid1, NULL, thr_fn1, NULL);
    if (err != 0)
        perror("can't create thread 1");
    err = pthread_create(&tid2, NULL, thr_fn2, NULL);
    if (err != 0)
        perror( "can't create thread 2");
  
    printf("thread 1 exit code %ld\n", (long)tret);
    printf("thread 2 exit code %ld\n", (long)tret);
    exit(0);
}
