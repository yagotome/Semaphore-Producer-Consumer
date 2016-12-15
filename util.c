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

void multiplicar(double **a, double **b, double **r, int n)
{
	int i,j,k;
	for (i=0; i < n; i++)
	{		
		for (j=0; j < n; j++)
		{            
            r[i][j] = 0;
			for (k=0; k < n; k++)
			{
                r[i][j] += a[i][k] * b[k][j];
			}
		}
	}
}

void determinante(double **c, double *det, int n)
{
	double a[n][n];
	int i, j, k, count=0;
	double factor, temp;
	for(i=0; i<n; i++) for(j=0; j<n; j++) a[i][j]=c[i][j];
	for(i = 0; i < n - 1; i++)
    {
        if(a[i][i] == 0)
        {
            for(k = i; k < n; k++)
            {
                if(a[k][i] != 0)
                {
                    for(j = 0; j < n; j++)
                    {
                        temp = a[i][j];
                        a[i][j] = a[k][j];
                        a[k][j] = temp;
                    }
                    k = n;
                }
            }
            count++;
        }

        if(a[i][i] != 0)
        {
            for(k = i + 1; k < n; k++)
            {
                factor = -1.0 * a[k][i] /  a[i][i];
                for(j = i; j < n; j++)
                {
                    a[k][j] = a[k][j] + (factor * a[i][j]);
                }
            }
        }
    }

    *det = 1.0;
    for(i = 0; i < n; i++)
	{
		*det *= a[i][i];
	}
	if(count%2 == 1)
		*det *= -1;
}
