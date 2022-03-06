#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>

#define THREAD_NUM 8
#define BUFFER_SIZE 5

int buffer[10];
int in = 0;
int out = 0;

void *producer(void *args)
{

    while (1)
    {
        int produced = rand() % 100;
        while ((in + 1) % BUFFER_SIZE == out)
        {
            // buffer is full, do nothing
            printf("Buffer is full: %d/%d slots are occupied\n",
                   in - out >= 0 ? (in - out) : BUFFER_SIZE - (out - in), BUFFER_SIZE);
            sleep(1);
        }

        buffer[in] = produced;
        in = (in + 1) % BUFFER_SIZE;
        printf("produced: %d, %d/%d slots are occupied\n", produced, 
                in - out >= 0 ? (in - out) : BUFFER_SIZE - (out - in), BUFFER_SIZE);
        sleep(2);
    }

    return 0;
}

void *consumer(void *args)
{
    while (1)
    {
        while (in == out)
        {
            // buffer is empty, do nothing
            printf("Buffer is empty: %d/%d slots are occupied\n",
                   in - out >= 0 ? (in - out) : BUFFER_SIZE - (out - in), BUFFER_SIZE);
            sleep(1);
        }
        int consumed = buffer[out];
        out = (out + 1) % BUFFER_SIZE;
        printf("consumed: %d, %d/%d slots are occupied\n", consumed, 
                in - out >= 0 ? (in - out) : BUFFER_SIZE - (out - in), BUFFER_SIZE);
        sleep(1);
    }

    return 0;
}

int main(int argc, char *argv[])
{
    srand(time(NULL));
    pthread_t p1, p2, p3, p4;
    if (pthread_create(&p1, NULL, &producer, NULL) != 0)
    {
        return 1;
    }
    if (pthread_create(&p2, NULL, &consumer, NULL) != 0)
    {
        return 2;
    }
    if (pthread_join(p1, NULL) != 0)
    {
        return 3;
    }
    if (pthread_join(p2, NULL) != 0)
    {
        return 4;
    }
    // printf("Number of mails: %d\n", buffer);
    return 0;
}