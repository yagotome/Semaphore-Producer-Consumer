#include <stdlib.h>
void alocar_matriz(double*** matriz, int n)
{
    *matriz = (double**) malloc(n * sizeof(double*));
    int i;
    for(i = 0; i < n; i++)
    {
        (*matriz)[i] = (double*) malloc(n * sizeof(double));
    }
}

void multiplicar(double **a, double **b, double ***r, int n)
{
	int i,j,k;	
	for (i=0; i < n; i++)
	{		
		for (j=0; j < n; j++)
		{
            *(r[i][j]) = 0;
			for (k=0; k < n; k++)
			{
				*(r[i][j]) += a[i][k]*b[k][j];
			}
		}
	}
}

void determinante(double **c, double *det, int n)
{
	double A[n][n];
	int i, j, k, factor, temp, count;
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
