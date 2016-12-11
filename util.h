#include <stdio.h>
#include <stdlib.h>
void abrirArq(FILE** arq, char* nome, char* modo); // ex: abrirArq(&arq, "arquivo.in", "r");
void lerProximoNomeDoCatalago(FILE* arq, char* proxNome);
void lerOrdemMatriz(FILE* arq, int* n);
void lerMatriz(FILE*, int** matriz, int n);
void desalocar(int*** matriz, int n);
void alocarMatriz (int*** mat, int n);
void multiplicarMatrizes(int** a, int** b, int** r, int n);
void determinante(int **c, int *det, int n);
