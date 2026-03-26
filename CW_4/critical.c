#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
int main(void)
{
	double x;
	int i;
	x = 0;

	#pragma omp parallel for shared(x)
	for(i=0; i<1000; i++)
	{
		#pragma omp critical
		{
			x = x + 1;
		}
	}

	printf("x = %f\n", x);
}
