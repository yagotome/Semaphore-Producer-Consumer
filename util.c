#include <stdio.h>
#include <stdlib.h>

void abrirArq (FILE **arq, char *nome, char *modo)
{
	if (!(*arq = fopen (nome, modo)))
	{
		fprintf(stderr, "Erro: arquivo com nome %s não existe\n", nome);
   		exit(1);
	}
}

void lerProximoNomeDoCatalago(FILE* arq, char* proxNome)
{
	if (fscanf(arq,"%s\n", proxNome) < 1)
	{
		fprintf(stderr, "Erro na leitura do catálogo\n");
		exit(1);
	}
}

void lerOrdemMatriz(FILE* arq, int* n)
{
    if (fscanf(arq,"%d\n", n) < 1)
	{
		fprintf(stderr, "Erro na leitura da ordem da matriz\n");
		exit(1);
	}
}

void lerMatriz (FILE* arq, int** mat, int n)
{
	int i,j;
	for (i=0; i < n; i++)
	{
		for (j=0; j < n; j++)
		{
            if (fscanf(arq, "%d", &(mat[i][j])) < 1)
            {
                fprintf(stderr, "Erro na leitura da matriz\n");
                exit(1);
            }
		}		
	}
}

void desalocar (int*** mat, int n)
{	
	int i;
	for (i=0; i < n; i++)
	{
		free((*mat)[i]);
	}	
	free(*mat);
}

void alocarMatriz (int*** mat, int n)
{
	if (!(*mat = (int**) malloc (n*sizeof(int*))))
	{
		fprintf(stderr, "Erro no malloc\n");
		exit(1);
	}		
	int i;
	for (i=0; i < n; i++)
	{
		if (!((*mat)[i] = (int*) malloc (n*sizeof(int))))
		{
			fprintf(stderr, "Erro no malloc\n");
			exit(1);
		}
	}	
}

void multiplicarMatrizes(int** a, int** b, int** r, int n)
{
	int i,j,k;	
	for (i=0; i < n; i++)
	{		
		for (j=0; j < n; j++)
		{	
            r[i][j] = 0;
			for (k=0; k < n; k++)
			{
				r[i][j] += a[i][k]*b[k][j];
			}
		}
	}
}

void determinante(int **c, int *det, int n)
{
	int A[n][n], i, j, k, factor, temp, count;
	for(i=0; i<n; i++)
	{
		for(j=0; j<n; j++)
		{
			A[i][j] = c[i][j];
		}
	}

	for(i=0; i < n-1; i++)
    {
        if(A[i][i] == 0)
        {
            for(k=i; k < n; k++)
            {
                if(A[k][i] != 0)
                {
                    for(j=0; j < n; j++)
                    {
                        temp = A[i][j];
                        A[i][j] = A[k][j];
                        A[k][j] = temp;
                    }
                    k = n;
                }
            }
            count++;
        }

        if(A[i][i] != 0)
        {
            for(k=i+1; k < n; k++)
            {
                factor = -1.0 * A[k][i] / A[i][i];
                for(j=i; j < n; j++)
                {
                    A[k][j] = A[k][j] + (factor * A[i][j]);
                }
            }
        }
	}

	for(i=0; i < n; i++)
	{
		*det *= A[i][i];
		if(count % 2 == 1) *det *= -1;
	}
}
