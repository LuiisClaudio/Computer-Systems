#include <stdlib.h>
#include "matriz.h"
#include "gauss.h"
#include <math.h>
#include <stdio.h>
#include "mmq.h"

double* mmq(int m, int n, double** A, double* b)
{
	double** T=mat_cria(n, m);
	double** C=mat_cria(n,n);
	double* Tb=(double*) malloc(n*sizeof(double));
	double* x=(double*) malloc(n*sizeof(double));
	int i, j;

	mat_transposta(m,n,A,T);
	mat_multm(n,m,n,T,A,C);
	mat_multv(n,m,T,b,Tb);

	gauss(n,C,Tb,x);

	return x;
}

double norma2 (int m, int n, double** A, double* b, double* x)
{
	double* r = (double*) malloc(m*sizeof(double));
	double* Ax = (double*) malloc(m*sizeof(double));
	double e = 0;
	int i;

	mat_multv(m,n,A,x,Ax);

	for(i=0;i<m;i++) {
		r[i] = b[i] - Ax[i];
		printf("r[%d]= %g\n", i+1, r[i]);
	}

	for(i=0;i<m;i++)
		e+=pow(r[i],2);
	e=pow(e,1.0/2.0);
	printf("%g\n", e);

	return e;
}