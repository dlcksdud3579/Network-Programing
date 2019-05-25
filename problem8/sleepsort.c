#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
void *thread_main(void *arg);

int start = 0;

int main(int argc, char *argv[])
{
    pthread_t* t_id;
    int i;
    t_id = (pthread_t*)malloc(sizeof(pthread_t)*argc);


    for (i = 1; i < argc; i++)
    {
        if (pthread_create(&(t_id[i]), NULL, thread_main, (void *)(argv[i])) != 0)
        {
            puts("pthread_create() erorr");
            return -1;
        }
    }
    start = 1;

    for (i = 1; i < argc; i++)
    {
        if (pthread_join(t_id[i],NULL) != 0)
        {
            puts("pthread_join() erorr");
            return -1;
        }
    }

    printf("\n");
    return 0;
}

void *thread_main(void *arg)
{
    while(start ==0);

    int n = atoi((char *)arg);
    usleep(n*1500);
    printf(" %d ",n);
    return NULL;
}