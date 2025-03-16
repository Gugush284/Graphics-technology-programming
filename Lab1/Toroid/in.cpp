#include <stdio.h>
#include <cmath>

int main() { 
	FILE *fp;
	int i,j,n;
	float r, R;
	double pi, alpha, beta, cosa, sina, x, x1, y1, z1, delta;

	printf("Input n:");
	scanf("%d", &n);
	printf("Input (R) and (r):");
	scanf("%f %f", &R, &r);

	fp= fopen("torus.dat","w");
	pi=4.0*atan(1.0); delta=2.0*pi/n;
	fprintf(fp, "%d\n", n); // количество точек объекта

    for (i=0; i<n; i++) {
        alpha=i*delta; cosa=cos(alpha); sina=sin(alpha);

        for (j=0; j<n; j++){
            beta=j*delta; x=R+r*cos(beta);
            x1=cosa*x; y1=sina*x; z1=r*sin(beta);
            fprintf(fp, "%d %f %f %f \n", i*n+j, x1, y1, z1);
        }
    }
    
    for (i=0; i<n; i++)
        for (j=0; j<n; j++)
            fprintf(fp, "%d %d %d %d\n", i*n+j, (i+1)%n*n+j, (i+1)%n*n+(j+1)%n, i*n+(j+1)%n);

    fclose(fp);
}
