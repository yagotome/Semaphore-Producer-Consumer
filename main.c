#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include </usr/include/semaphore.h>
#include "util.h"

#define BUFF_SIZE   5       /* total number of BUFF_SIZE */
#define nLC         1		/* total number of LCs */
#define nLA         3		/* total number of LAs */
#define nMM         4		/* total number of MMs */
#define nDM         5		/* total number of DMs */
#define nEA         3		/* total number of EAs */
#define nSHARED     4       /* number of shared sbuf_t */

typedef struct {
    char nome[15];
    int size;
    double **a;
    double **b;
    double **c;
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

sbuf_t shared[nSHARED];
int ent=0, sai=0;
sem_t mutex;

void *LC(void *arg)
{
	char nome[15];
	FILE *arq = fopen ("in/entrada.in", "r");
	while(fscanf(arq, "%s\n", nome))
	{
		sem_wait(&shared[0].empty);
        sem_wait(&shared[0].mutex);

		strcpy(shared[0].buffer[shared[0].in].nome, nome); 
		ent++;
		shared[0].in = (shared[0].in+1) % BUFF_SIZE;

		sem_post(&shared[0].mutex);
        sem_post(&shared[0].full);
	}

	return NULL;
}

void *LA(void *arg)
{
	while(true)
	{
		int i, j;

		sem_wait(&shared[0].full);
        sem_wait(&shared[0].mutex);

		FILE *arq = fopen(strcat(shared[0].buffer[shared[0].out].nome, ".txt"), "r");		
		fscanf(arq, "%d", &(shared[0].buffer[shared[0].out].size));
		
		shared[0].buffer[shared[0].out].a = (double **) malloc(sizeof(double *));
		shared[0].buffer[shared[0].out].b = (double **) malloc(sizeof(double *));
		shared[0].buffer[shared[0].out].c = (double **) malloc(sizeof(double *));
		for(i=0; i<shared[0].buffer[shared[0].out].size; i++)
		{
			shared[0].buffer[shared[0].out].a[i] = (double *) malloc(sizeof(double));
			shared[0].buffer[shared[0].out].b[i] = (double *) malloc(sizeof(double));
			shared[0].buffer[shared[0].out].c[i] = (double *) malloc(sizeof(double));
		}


		for(i=0; i<shared[0].buffer[shared[0].out].size; i++)
		{
			for(j=0; j<shared[0].buffer[shared[0].out].size; j++)
			{
				fscanf(arq, "%d", &(shared[0].buffer[shared[0].out].a[i][j]));
			}
		}
		for(i=0; i<shared[0].buffer[shared[0].out].size; i++)
		{
			for(j=0; j<shared[0].buffer[shared[0].out].size; j++)
			{
				fscanf(arq, "%d", &(shared[0].buffer[shared[0].out].b[i][j]));
			}
		}

		sem_wait(&shared[1].mutex);
		sem_wait(&shared[1].empty);

		shared[1].buffer[shared[1].in] = shared[0].buffer[shared[0].out];
		shared[1].in = (shared[1].in+1) % BUFF_SIZE;
		shared[0].out = (shared[0].out+1) % BUFF_SIZE;

		sem_post(&shared[1].mutex);
		sem_post(&shared[1].full);
		sem_post(&shared[0].mutex);
        sem_post(&shared[0].empty);
	}
}

void *MM(void *arg)
{
	while(true)
	{
		sem_wait(&shared[1].mutex);
		sem_wait(&shared[1].full);

		multiplicar(shared[1].buffer[shared[1].out].a, shared[1].buffer[shared[1].out].b, &(shared[1].buffer[shared[1].out].c), shared[1].buffer[shared[1].out].size);

		sem_wait(&shared[2].mutex);
		sem_wait(&shared[2].empty);

		shared[2].buffer[shared[2].in] = shared[1].buffer[shared[1].out];
		shared[2].in = (shared[2].in+1) % BUFF_SIZE;
		shared[1].out = (shared[1].out+1) % BUFF_SIZE;

		sem_post(&shared[2].mutex);
		sem_post(&shared[2].full);
		sem_post(&shared[1].mutex);
        sem_post(&shared[1].empty);
	}
}

void *DM(void *arg)
{
	while(true)
	{
		sem_wait(&shared[2].mutex);
		sem_wait(&shared[2].full);

		determinante(shared[2].buffer[shared[2].out].c, &(shared[2].buffer[shared[2].out].det), shared[2].buffer[shared[2].out].size);

		sem_wait(&shared[3].mutex);
		sem_wait(&shared[3].empty);

		shared[3].buffer[shared[3].in] = shared[2].buffer[shared[2].out];
		shared[3].in = (shared[3].in+1) % BUFF_SIZE;
		shared[2].out = (shared[2].out+1) % BUFF_SIZE;

		sem_post(&shared[3].mutex);
		sem_post(&shared[3].full);
		sem_post(&shared[2].mutex);
        sem_post(&shared[2].empty);
	}
}

void *EA(void *arg)
{
	while(true)
	{
		sem_wait(&shared[3].mutex);
		sem_wait(&shared[3].full);

		FILE *arq = fopen(strcat(shared[3].buffer[shared[3].out].nome, ".out"), "w");
		fprintf(arq, "================================\n%s.txt\n--------------------------------\nA\n", shared[3].buffer[shared[3].out].nome);
		int i, j;
		for(i=0; i<shared[3].buffer[shared[3].out].size; i++)
		{
			for(j=0; j<shared[3].buffer[shared[3].out].size; j++)
			{
				fprintf(arq, "%d ", shared[3].buffer[shared[3].out].a[i][j]);
			}
			fprintf(arq, "\n");
		}
		fprintf(arq, "B\n");
		for(i=0; i<shared[3].buffer[shared[3].out].size; i++)
		{
			for(j=0; j<shared[3].buffer[shared[3].out].size; j++)
			{
				fprintf(arq, "%d ", shared[3].buffer[shared[3].out].b[i][j]);
			}
			fprintf(arq, "\n");
		}
		fprintf(arq, "C\n");
		for(i=0; i<shared[3].buffer[shared[3].out].size; i++)
		{
			for(j=0; j<shared[3].buffer[shared[3].out].size; j++)
			{
				fprintf(arq, "%d ", shared[3].buffer[shared[3].out].c[i][j]);
			}
			fprintf(arq, "\n");
		}
		fprintf(arq, "================================");

		for(i=shared[3].buffer[shared[3].out].size - 1; i>=0; i--)
		{
			free(shared[3].buffer[shared[3].out].a[i]);
			free(shared[3].buffer[shared[3].out].b[i]);
			free(shared[3].buffer[shared[3].out].c[i]);
		}
		free(shared[3].buffer[shared[3].out].a);
		free(shared[3].buffer[shared[3].out].b);
		free(shared[3].buffer[shared[3].out].c);

		sem_wait(mutex);
		sai++;  // errado. consertar.
		sem_post(mutex);

		shared[3].out = (shared[3].out+1) % BUFF_SIZE;

		sem_post(&shared[3].mutex);
        sem_post(&shared[3].empty);
	}
}

int main()
{
    pthread_t idLC, idLA, idMM, idDM, idEA;
    int i;
    int sLC[nLC], sLA[nLA], sEA[nMM], sLA[nDM], sEA[nEA];

    for(i=0; i<nSHARED; i++)
    {
    	sem_init(&shared[i].full, 0, 0);
    	sem_init(&shared[i].empty, 0, BUFF_SIZE);
    	sem_init(&shared[i].mutex, 0, 1);
    }

    for(i=0; i<nLC; i++)
    {
		sLC[i]=i;
		pthread_create(&idLC, NULL, LC, &sLC[i]);
    }

    for(i=0; i<nLA; i++)
    {  
		sLA[i]=i;
		pthread_create(&idLA, NULL, LA, &sLA[i]);
    }

	for(i=0; index < nMM; index++)
	{
		sMM[i]=i;
		pthread_create(&idMM, NULL, EA, &sMM[i]);
	}

	for(i=0; i<nDM; i++)
	{
		sDM[i]=i;
		pthread_create(&idDM, NULL, EA, &sDM[i]);
	}

    for(i=0; i<nEA; i++)
    {  
		sEA[i]=i;
		pthread_create(&idEA, NULL, EA, &sEA[i]);
    }

    pthread_exit(NULL);
}
