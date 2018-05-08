#include <stdlib.h>
#include "matriz.h"

double** mat_cria (int m, int n)
{
	int i;
	double** matriz;
	matriz = (double**) malloc(m*sizeof(double*));
	
	for(i=0;i<m;i++) {
		matriz[i] = (double*) malloc(n*sizeof(double));
	}

	return matriz;
}

void mat_libera (int m, double** A)
{
	int i;

	for(i=0;i<m;i++) {
		free(A[i]);
	}

	free(A);
}

void mat_transposta (int m, int n, double** A, double** T)
{
	int i, j;

	for(i=0;i<n;i++) {
		for(j=0;j<m;j++) {
			T[i][j] = A[j][i];
		}
	}
}

void mat_multv (int m, int n, double** A, double* v, double* w)
{
	int i, j;
	double soma;

	for(i=0;i<m;i++) {
		soma = 0;
		for(j=0;j<n;j++) {
			soma += A[i][j] * v[j];
		}
		w[i] = soma;
	}
}

void mat_multm (int m, int n, int q, double** A, double** B, double** C)
{
	int i, j, k, l;
	double soma;

	for(i=0;i<m;i++) {
		k = 0;
		for(l=0;l<q;l++) {
			soma = 0;
			for(j=0;j<n;j++) {
				soma += A[i][j] * B[j][l];
			}
			C[i][l] = soma;
		}
	}
}