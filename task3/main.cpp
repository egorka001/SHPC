#include <iostream>
#include <omp.h>
#include <ctime>

#define MAX 500 

void seq_mult(int32_t A[MAX][MAX], int32_t B[MAX][MAX], int32_t C[MAX][MAX])
{
	uint32_t i, j, k;
	for(i = 0; i < MAX; ++i)
		for(j = 0; j < MAX; ++j)
			for(k = 0; k < MAX; ++k)
				C[i][j] += A[i][k] * B[k][j];
}

void par_mult(int32_t A[MAX][MAX], int32_t B[MAX][MAX], int32_t C[MAX][MAX])
{
	uint32_t i, j, k;
	#pragma omp parallel for private(i, j, k)
	for(i = 0; i < MAX; ++i)
		for(j = 0; j < MAX; ++j)
			for(k = 0; k < MAX; ++k)
				C[i][j] += A[i][k] * B[k][j];
}

bool check(int32_t A[MAX][MAX], int32_t B[MAX][MAX])
{
	for(uint32_t i = 0; i < MAX; ++i)
		for(uint32_t j = 0; j < MAX; ++j)
			if(A[i][j] != B[i][j])
				return 0;
	return 1;
}

int main()
{
	int32_t A[MAX][MAX];
	int32_t B[MAX][MAX];
	int32_t C_par[MAX][MAX];
	int32_t C_seq[MAX][MAX];

	double start, stop, time_par, time_seq;

	srand(time(NULL));

	for(uint32_t i = 0; i < MAX; ++i)
	{
		for(uint32_t j = 0; j < MAX; ++j)
		{
			A[i][j] = rand() % 21 - 10;
			B[i][j] = rand() % 21 - 10;
		}
	}
	
	start = omp_get_wtime();
	seq_mult(A, B, C_seq);
	stop = omp_get_wtime();
	time_seq = stop - start;

	start = omp_get_wtime();
	par_mult(A, B, C_par);
	stop = omp_get_wtime();
	time_par = stop - start;

	if(check(C_par, C_seq))
	{
		std::cout << "TimeSeq: " << time_seq << " seconds\n";
		std::cout << "TimePar: " << time_par << " seconds\n";
	}

	return 0;
}
