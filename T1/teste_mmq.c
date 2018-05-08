#include <stdlib.h>
#include <unistd.h>
#include "matriz.h"
#include <stdio.h>
#include <math.h>
#include "mmq.h"

void exibe_matriz(int m, int n, double** A)
{
	int i, j;

	for(i=0;i<m;i++) {
		for(j=0;j<n;j++)
			printf("%3g ", A[i][j]);
		printf("\n");
	}
}

int main(void)
{
	double **A, **B, **E;
	double c[5]={10,10,-5,15,0}, d[5]={10,0,2,0,5}, f[8]={8.0,12.3,15.5,16.8,17.1,15.8,15.2,14.0};
	double g[8];
	double *x, j;
	int i;

  for(;;) {
	  A = mat_cria(5,3);
	  B = mat_cria(5,4);

	  A[0][0]=3; A[0][1]=-1; A[0][2]=2;
	  A[1][0]=4; A[1][1]=1; A[1][2]=0;
	  A[2][0]=-3; A[2][1]=2; A[2][2]=1;
	  A[3][0]=1; A[3][1]=1; A[3][2]=5;
	  A[4][0]=-2; A[4][1]=0; A[4][2]=3;

	  B[0][0]=4; B[0][1]=2; B[0][2]=3; B[0][3]=0;
	  B[1][0]=-2; B[1][1]=3; B[1][2]=-1; B[1][3]=1;
	  B[2][0]=1; B[2][1]=3; B[2][2]=-4; B[2][3]=2;
	  B[3][0]=1; B[3][1]=0; B[3][2]=1; B[3][3]=-1;
	  B[4][0]=3; B[4][1]=1; B[4][2]=3; B[4][3]=-2;

	  x = mmq(5,3,A,c);

	  free(x);
	  mat_libera(5,A);

	  x = mmq(5,4,B,d);

	  free(x);
	  mat_libera(5,B);

	  for(i=0;i<8;i++)
		  g[i]=log(f[i]) - log(1.0*(i+1));

	  E=mat_cria(8,2);
	  for(i=0;i<8;i++) {
		  E[i][0]=1;
		  E[i][1]=i+1;
	  }
	  x=mmq(8,2,E,g);

	  free(x);
	  mat_libera(8,E);
	  usleep(500000);
  }
  
	return 0;
}
