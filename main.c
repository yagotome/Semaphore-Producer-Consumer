/* Autores: 
Lucas Alves de Sousa
Yago Gomes Tomé de Sousa */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include </usr/include/semaphore.h>
#include "util.h"

#define BUFF_SIZE   5
#define nLC         1
#define nLA         3
#define nMM         4
#define nDM         5
#define nEA         3
#define nSHARED     4

typedef struct {
    char nome[15];
    int size;
    double **a;
    double **b;
    double **c;
    double det;
} S;

typedef struct {
    S buffer[BUFF_SIZE];
    int in;
    int out;
    sem_t full;
    sem_t empty;
    sem_t mutex;
} sbuf_t;

sbuf_t shared[nSHARED];
int ent = 0, sai = 0, terminouLeitura=0;
sem_t mutex;

void *LC(void *arg)
{
	S temp;
	FILE *arq = fopen ("in/entrada.in", "r");
	while(fscanf(arq, "%s\n", temp.nome) == 1)
	{		
		sem_wait(&shared[0].empty);
        sem_wait(&shared[0].mutex);
		
		shared[0].buffer[shared[0].in] = temp; 
		shared[0].in = (shared[0].in+1) % BUFF_SIZE;
		ent++;

		sem_post(&shared[0].mutex);
        sem_post(&shared[0].full);
		
	}

	terminouLeitura = 1;

	return NULL;
}

void *LA(void *arg)
{
	while(1)
	{
		sem_wait(&shared[0].full);
        sem_wait(&shared[0].mutex);

		S temp = shared[0].buffer[shared[0].out];
		shared[0].out = (shared[0].out+1) % BUFF_SIZE;

		sem_post(&shared[0].mutex);
        sem_post(&shared[0].empty);

		char in_dir[50] = "in/";
		strcat(in_dir, temp.nome);
		strcat(in_dir, ".in");
		FILE *arq = fopen(in_dir, "r");		
		fscanf(arq, "%d", &(temp.size));		
		
		int n = temp.size;

		alocar_matriz(&temp.a, n);
		alocar_matriz(&temp.b, n);

		int i, j;
		for(i = 0; i < n; i++)
		{
			for(j = 0; j < n; j++)
			{
				fscanf(arq, "%lf", &(temp.a[i][j]));
			}
		}
		for(i = 0; i < n; i++)
		{
			for(j = 0; j < n; j++)
			{
				fscanf(arq, "%lf", &(temp.b[i][j]));
			}
		}

		sem_wait(&shared[1].empty);
		sem_wait(&shared[1].mutex);

		shared[1].buffer[shared[1].in] = temp;	
		shared[1].in = (shared[1].in+1) % BUFF_SIZE;

		sem_post(&shared[1].mutex);
		sem_post(&shared[1].full);
	}
}

void *MM(void *arg)
{
	while(1)
	{
		sem_wait(&shared[1].full);
		sem_wait(&shared[1].mutex);
		
		S temp = shared[1].buffer[shared[1].out];
		shared[1].out = (shared[1].out+1) % BUFF_SIZE;
		
		sem_post(&shared[1].mutex);
        sem_post(&shared[1].empty);

		alocar_matriz(&temp.c, temp.size);
		multiplicar(temp.a, temp.b, temp.c, temp.size);

		sem_wait(&shared[2].empty);
		sem_wait(&shared[2].mutex);

		shared[2].buffer[shared[2].in] = temp;		
		shared[2].in = (shared[2].in+1) % BUFF_SIZE;

		sem_post(&shared[2].mutex);
		sem_post(&shared[2].full);
	}
}

void *DM(void *arg)
{
	while(1)
	{
		sem_wait(&shared[2].full);
		sem_wait(&shared[2].mutex);

		S temp = shared[2].buffer[shared[2].out];
		shared[2].out = (shared[2].out+1) % BUFF_SIZE;

		sem_post(&shared[2].mutex);
        sem_post(&shared[2].empty);

		determinante(temp.c, &(temp.det), temp.size);

		sem_wait(&shared[3].empty);
		sem_wait(&shared[3].mutex);
		
		shared[3].buffer[shared[3].in] = temp;
		shared[3].in = (shared[3].in+1) % BUFF_SIZE;

		sem_post(&shared[3].mutex);
		sem_post(&shared[3].full);
	}
}

void *EA(void *arg)
{
	while(1)
	{
		sem_wait(&shared[3].full);
		sem_wait(&shared[3].mutex);
		
		S temp = shared[3].buffer[shared[3].out];
		shared[3].out = (shared[3].out+1) % BUFF_SIZE;

		sem_post(&shared[3].mutex);
        sem_post(&shared[3].empty);		

		char out_dir[50] = "out/";
		strcat(out_dir, temp.nome);
		strcat(out_dir, ".out");
		FILE *arq = fopen(out_dir, "w");
		fprintf(arq, "================================\n%s.txt\n--------------------------------\nA\n", temp.nome);
		int i, j;
		for(i = 0; i<temp.size; i++)
		{
			for(j = 0; j<temp.size; j++)
			{
				fprintf(arq, "%lf ", temp.a[i][j]);
			}
			fprintf(arq, "\n");
		}
		fprintf(arq, "--------------------------------\nB\n");
		for(i = 0; i<temp.size; i++)
		{
			for(j = 0; j<temp.size; j++)
			{
				fprintf(arq, "%lf ", temp.b[i][j]);
			}
			fprintf(arq, "\n");
		}
		fprintf(arq, "--------------------------------\nC\n");
		for(i = 0; i<temp.size; i++)
		{
			for(j = 0; j<temp.size; j++)
			{
				fprintf(arq, "%lf ", temp.c[i][j]);
			}
			fprintf(arq, "\n");
		}
		fprintf(arq, "--------------------------------\n%lf\n================================", temp.det);

		for(i=temp.size - 1; i >= 0; i--)
		{
			free(temp.a[i]);
			free(temp.b[i]);
			free(temp.c[i]);
		}
		free(temp.a);
		free(temp.b);
		free(temp.c);

		sem_wait(&mutex);
		sai++;
		sem_post(&mutex);
	}
}

int main()
{
    pthread_t idLC, idLA, idMM, idDM, idEA;
    int i, j;
    int sLC[nLC], sLA[nLA], sMM[nMM], sDM[nDM], sEA[nEA];

    for(i = 0; i<nSHARED; i++)
    {
    	sem_init(&shared[i].full, 0, 0);
    	sem_init(&shared[i].empty, 0, BUFF_SIZE);
    	sem_init(&shared[i].mutex, 0, 1);
    }

    for(i = 0; i<nLC; i++)
    {
		sLC[i]=i;
		pthread_create(&idLC, NULL, LC, &sLC[i]);
    }

    for(i = 0; i<nLA; i++)
    {  
		sLA[i]=i;
		pthread_create(&idLA, NULL, LA, &sLA[i]);
    }

	for(i = 0; i<nMM; i++)
	{
		sMM[i]=i;
		pthread_create(&idMM, NULL, MM, &sMM[i]);
	}

	for(i = 0; i<nDM; i++)
	{
		sDM[i]=i;
		pthread_create(&idDM, NULL, DM, &sDM[i]);
	}

	sem_init(&mutex, 0, 1);
    for(i = 0; i<nEA; i++)
    {  
		sEA[i]=i;
		pthread_create(&idEA, NULL, EA, &sEA[i]);
    }

	while(1)
	{
		if(ent == sai && terminouLeitura) exit(0);
	}
}
