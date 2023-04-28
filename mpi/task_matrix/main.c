#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

//N x N matrix, N = 2^t
const int N = 512; 

//function that checks equality of two matrix
bool check(int A[N * N], int B[N * N])
{
	for (int i = 0; i < N; ++i)
		for (int j = 0; j < N; ++j)
			if (A[i * N + j] != B[i * N + j])
				return 0;
	return 1;
}

//fuction for printing matrix in terminal
void out_m(int A[N * N])
{
	for (int i = 0; i < N; ++i)
	{
		for(int j = 0; j < N; ++j)
			printf("%d \t", A[i * N + j]);
		printf("\n");
	}
	printf("-------------------\n");
}

int main(int argc, char** argv) 
{
    int nranks, rank;

    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &nranks);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	int buf_N = N * N / nranks;
	int A[N * N], B[N * N], C_seq[N * N];
	int A_buf[buf_N], C_buf[buf_N], C_par[N * N];
	double t_start_seq, t_stop_seq;
	double t_start_par, t_stop_par;

	if (rank == 0)
	{
		//matrixs init
		srandom(time(NULL));
		for (int i = 0; i < N; ++i)
		{
			for (int j = 0; j < N; ++j)
			{
				A[i * N + j] = random() % 21 - 10;
				B[i * N + j] = random() % 21 - 10;
				C_seq[i * N + j] = 0;
				C_par[i * N + j] = 0;
			}
		}

		for (int i = 0; i < buf_N; ++i)
			C_buf[i] = 0;
	}	

	//sync all threads to get correct time
	MPI_Barrier(MPI_COMM_WORLD);
	t_start_par = MPI_Wtime();

	//scatter rows from A to threads
	MPI_Scatter(&A, buf_N, MPI_INT, &A_buf, buf_N, MPI_INT, 0, MPI_COMM_WORLD);
	//share all data
	MPI_Bcast(&B, N * N, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&C_buf, buf_N, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&C_par, N * N, MPI_INT, 0, MPI_COMM_WORLD);

	//calculate A rows * B cols in C buf
	for (int i = 0; i < buf_N / N; ++i)
		for (int j = 0; j < N; ++j)
			for (int k = 0; k < N; ++k)
				C_buf[i * N + j] += A_buf[i * N + k] * B[k * N + j];

	//gather result matrix C
	MPI_Gather(&C_buf, buf_N, MPI_INT, &C_par, buf_N, MPI_INT, 0, MPI_COMM_WORLD);
	//sync all threads to get correct time
	MPI_Barrier(MPI_COMM_WORLD);
	t_stop_par = MPI_Wtime();

	if(rank == 0)
	{
		//seq algorithm
		t_start_seq = MPI_Wtime();
		for (int i = 0; i < N; ++i)
			for (int j = 0; j < N; ++j)
				for (int k = 0; k < N; ++k)
					C_seq[i * N + j] += A[i * N + k] * B[k * N + j];
		t_stop_seq = MPI_Wtime();

		if(check(C_seq, C_par))
		{
			printf("RANKS: %d\n", nranks);
			printf("Matrix size: %d\n", N);
			printf("Seq time: %f\n", t_stop_seq - t_start_seq);
			printf("Par time: %f\n", t_stop_par - t_start_par);
		}		
	}

    MPI_Finalize();

	return 0;
}

