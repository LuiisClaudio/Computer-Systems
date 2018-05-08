#include "gauss.h"
#include "matriz.h"
#include <math.h>
#include <stdio.h>

void gauss (int n, double** A, double* b, double* x)
{
	int i, j, k, p;
	double fator, s;

	for (j=0;j<=n-2;j++) {
		for(i=j+1;i<=n-1;i++) {
			fator = A[i][j] / A[j][j];
			if(fabs(fator) >= 1 ) {
				//Inicio pivotamento
				p = j;
				for (k=j+1;k<=n-1;k++) {
					if (fabs(A[k][j]) > fabs(A[p][j])) {
						p = k;
					}
				} //Fim pivotamento
				//Swap
				for (k=j;k<=n-1;k++) {
					fator = A[j][k];
					A[j][k] = A[p][k];
					A[p][k] = fator;

					fator = b[j];
					b[j] = b[k];
					b[k] = fator;
				}
				//Fim swap
				fator = A[i][j] / A[j][j];
			}
			for (k=j;k<=n-1;k++) {
				A[i][k] -= A[j][k] * fator;
			}
			b[i] -= b[j] * fator;
		}
	}

	for (i=n-1;i>=0;i--) {
		s = 0;
		for (j=i+1;j<=n-1;j++) {
			s += (A[i][j] * x[j]);
		}
		x[i] = (b[i] - s) / A[i][i];
	}
}