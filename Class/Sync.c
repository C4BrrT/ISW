#include <pthread.h>
#include <semaphore.h>
#include <dispatch/dispatch.h>
#include <stdio.h>
#include <stdlib.h>
//comments = macOS

pthread_mutex_t mutex;
sem_t full; //Linux
//dispatch_semaphore_t sem;
sem_t empty;
//dispatch_semaphore_t empty; 

void *producer(void *pno);
void *consumer(void *cno);

int *buffer;
int in=0;
int out=0;

int B;

int main(){
    int P=10;
    int C=10;
    B=15;

    pthread_t pro = (pthread_t*)malloc(sizeof(pthread_t) * P);
    pthread_t con = (pthread_t*)malloc(sizeof(pthread_t) * C);

    buffer = (int *)malloc(sizeof(int) * B);

    pthread_mutex_init(&mutex, NULL);

    sem_init(&full, 0, 0);
    //full = dispatch_semaphore_create(0);

    sem_init(&empty, 0, B);
    //empty = dispatch_semaphore_create(B);

    int size = P>C?P:C;

    int* a = (int *)malloc(sizeof(int) * size);
    for(int i=0; i<size; i++){
        a[i] = i;
    }

    for(int i=0; i<P; i++){
        pthread_create(&pro[i], NULL, producer, &a[i]);
    }

    for(int i=0; i<C; i++){
        pthread_create(&con[i], NULL, consumer, &a[i]);
    }

    for(int i=0; i<P; i++){
        pthread_join(pro[i], NULL);
    }
    
    for(int i=0; i<C; i++){
        pthread_join(con[i], NULL);
    }

    return 0;
}

void producer(void *pno){
    int* id = pno;
    int item;
    while(1){
        for(int i=0; i<10; i++){
            item = 2*i+1;
            sem_wait(&full);
            //dispatch_semaphore_wait(empty, DISPATCH_TIME_FOREVER);
            pthread_mutex_lock(&mutex);
            buffer[in] = *id;
            printf("Producer %d: Insert Item %d at %d", *id, item, in);
            in = (in+1)%B;
            pthread_mutex_unlock(&mutex);
            sem_post(&full);
            //dispatch_semaphore_signal(full);
        }
    }
}

void *consumer(void *cno){
    int* id = cno;
    while(1){
        sem_wait(&full);
        //dispatch_semaphore_wait(full, DISPATCH_TIME_FOREVER);
        pthread_mutex_lock(&mutex);
        item = buffer[out];
        printf("Consumer %d: consuming Item %d from %d\n", *id, item, out);
        out = (out+1)%B;
        pthread_mutex_unlock(&mutex);
        sem_post(&empty);
    }
}