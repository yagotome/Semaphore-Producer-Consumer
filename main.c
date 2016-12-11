#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include </usr/include/semaphore.h>
#include "util.h"

#define BUFF_SIZE   5		/* total number of slots */
#define NLC         1		/* total number of LCs */
#define NLA         3		/* total number of LAs */
#define NMM         4		/* total number of MMs */
#define NDM         5		/* total number of DMs */
#define NEA         3		/* total number of EAs */
#define NITERS      4		/* number of items produced/consumed */
#define NSHARED     4       /* number of shared sbuf_t */

typedef struct {
    char nome[80];
    int size;
    double *a;
    double *b;
    double *c;
    double det;
} S;

typedef struct {
    S buffer[BUFF_SIZE];   /* shared var */
    int in;         	  /* buffer[in%BUFF_SIZE] is the first empty slot */
    int out;        	  /* buffer[out%BUFF_SIZE] is the first full slot */
    sem_t full;     	  /* keep track of the number of full spots */
    sem_t empty;    	  /* keep track of the number of empty spots */
    sem_t mutex;    	  /* enforce mutual exclusion to shared data */
} sbuf_t;

sbuf_t shared[NSHARED];

void *LC(void *arg)
{
    int i, item, index;

    index = *((int *)arg);

    for (i=0; i < NITERS; i++) {

        /* Produce item */
        item = i+(index*1000);	

        /* Prepare to write item to buffer */

        /* If there are no empty slots, wait */
        sem_wait(&shared[0].empty);
        /* If another thread uses the buffer, wait */
        sem_wait(&shared[0].mutex);
        shared[0].buffer[shared[0].in] = item;
        shared[0].in = (shared[0].in+1)%BUFF_SIZE;
        printf("[P%d] Producing %d ...\n", index, item); fflush(stdout);
        /* Release the buffer */
        sem_post(&shared[0].mutex);
        /* Increment the number of full slots */
        sem_post(&shared[0].full);
    }
    return NULL;
}

void *LA(void *arg)
{
    int i, item, index;

    index = *((int *)arg);

    for (i=0; i < NITERS; i++) {

	/* Prepare to read item from buffer */

        /* If there are no filled slots, wait */
        sem_wait(&shared[0].full);
        /* If another thread uses the buffer, wait */
        sem_wait(&shared[0].mutex);
        item = shared[0].buffer[shared[0].out];
        shared[0].out = (shared[0].out+1)%BUFF_SIZE;
        printf("[CP%d] Consuming %d ...\n", index, item); fflush(stdout);
        /* Release the buffer */
        sem_post(&shared[0].mutex);
        /* Increment the number of empty slots */
        sem_post(&shared[0].empty);

	//Change consumed item
	item += 500;

        /* Prepare to write item to buffer */

        /* If there are no empty slots, wait */
        sem_wait(&shared[1].empty);
        /* If another thread uses the buffer, wait */
        sem_wait(&shared[1].mutex);
        shared[1].buffer[shared[1].in] = item;
        shared[1].in = (shared[1].in+1)%BUFF_SIZE;
        printf("[CP%d] Producing %d ...\n", index, item); fflush(stdout);
        /* Release the buffer */
        sem_post(&shared[1].mutex);
        /* Increment the number of full slots */
        sem_post(&shared[1].full);
	
    }
    return NULL;
}

void *MM(void *arg)
{
    int i, item, index;

    index = *((int *)arg);

    for (i=0; i < NITERS; i++) {

	/* Prepare to read item from buffer */

        /* If there are no filled slots, wait */
        sem_wait(&shared[0].full);
        /* If another thread uses the buffer, wait */
        sem_wait(&shared[0].mutex);
        item = shared[0].buffer[shared[0].out];
        shared[0].out = (shared[0].out+1)%BUFF_SIZE;
        printf("[CP%d] Consuming %d ...\n", index, item); fflush(stdout);
        /* Release the buffer */
        sem_post(&shared[0].mutex);
        /* Increment the number of empty slots */
        sem_post(&shared[0].empty);

	//Change consumed item
	item += 500;

        /* Prepare to write item to buffer */

        /* If there are no empty slots, wait */
        sem_wait(&shared[1].empty);
        /* If another thread uses the buffer, wait */
        sem_wait(&shared[1].mutex);
        shared[1].buffer[shared[1].in] = item;
        shared[1].in = (shared[1].in+1)%BUFF_SIZE;
        printf("[CP%d] Producing %d ...\n", index, item); fflush(stdout);
        /* Release the buffer */
        sem_post(&shared[1].mutex);
        /* Increment the number of full slots */
        sem_post(&shared[1].full);
	
    }
    return NULL;
}

void *MD(void *arg)
{
    int i, item, index;

    index = *((int *)arg);

    for (i=0; i < NITERS; i++) {

	/* Prepare to read item from buffer */

        /* If there are no filled slots, wait */
        sem_wait(&shared[0].full);
        /* If another thread uses the buffer, wait */
        sem_wait(&shared[0].mutex);
        item = shared[0].buffer[shared[0].out];
        shared[0].out = (shared[0].out+1)%BUFF_SIZE;
        printf("[CP%d] Consuming %d ...\n", index, item); fflush(stdout);
        /* Release the buffer */
        sem_post(&shared[0].mutex);
        /* Increment the number of empty slots */
        sem_post(&shared[0].empty);

	//Change consumed item
	item += 500;

        /* Prepare to write item to buffer */

        /* If there are no empty slots, wait */
        sem_wait(&shared[1].empty);
        /* If another thread uses the buffer, wait */
        sem_wait(&shared[1].mutex);
        shared[1].buffer[shared[1].in] = item;
        shared[1].in = (shared[1].in+1)%BUFF_SIZE;
        printf("[CP%d] Producing %d ...\n", index, item); fflush(stdout);
        /* Release the buffer */
        sem_post(&shared[1].mutex);
        /* Increment the number of full slots */
        sem_post(&shared[1].full);
	
    }
    return NULL;
}

void *EA(void *arg)
{
    int i, item, index;

    index = *((int *)arg);

    for (i=0; i < NITERS; i++) {

        /* Prepare to read item from buffer */

        /* If there are no filled slots, wait */
        sem_wait(&shared[1].full);
        /* If another thread uses the buffer, wait */
        sem_wait(&shared[1].mutex);
        item = shared[1].buffer[shared[1].out];
        shared[1].out = (shared[1].out+1)%BUFF_SIZE;
        printf("[C%d] Consuming %d ...\n", index, item); fflush(stdout);
        /* Release the buffer */
        sem_post(&shared[1].mutex);
        /* Increment the number of empty slots */
        sem_post(&shared[1].empty);
	
    }
    return NULL;
}

int main()
{
    pthread_t idP, idC, idCP;
    int index;
    int sLC[NLC], sEA[NEA], sLA[NLA];
    for (index=0; index < NSHARED; index++)
    {
    	sem_init(&shared[index].full, 0, 0);
    	sem_init(&shared[index].empty, 0, BUFF_SIZE);
    	sem_init(&shared[index].mutex, 0, 1);
    }

    for (index = 0; index < NLC; index++)
    {
       sLC[index]=index;
       /* Create a new LC */
       pthread_create(&idP, NULL, LC, &sLC[index]);
    }

    for (index = 0; index < NLA; index++)
    {  
       sLA[index]=index;
       /* Create a new LA */
       pthread_create(&idCP, NULL, LA, &sLA[index]);
    }

    for (index = 0; index < NEA; index++)
    {  
       sEA[index]=index;
       /* Create a new EA */
       pthread_create(&idC, NULL, EA, &sEA[index]);
    }

    pthread_exit(NULL);
}

