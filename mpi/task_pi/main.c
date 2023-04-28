#include <mpi.h>
#include <stdio.h>

int main(int argc, char** argv) 
{
    int nranks;
    int rank;

	int N = 1e6;
	double x, sum_t = 0, sum_up;
	double t_start, t_stop;

    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &nranks);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	//sync all threads to get correct time
	MPI_Barrier(MPI_COMM_WORLD);
	t_start = MPI_Wtime();

	//split sum from 0 to N to diffent threads
    for (int i = rank; i < N; i += nranks)
    {
        x = (i + 0.5) / N;
		sum_t += 4 / (1 + x * x);
    }

	//sum all splitted sums
	MPI_Reduce(&sum_t, &sum_up, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

	//sync all threads to get correct time
	MPI_Barrier(MPI_COMM_WORLD);
	t_stop = MPI_Wtime();

	if (rank == 0)
	{
		printf("NRANKS: %d\n", nranks);

		//make one output for parralel algorithm
		sum_up /= N;
		printf("Par time: %f; ", t_stop - t_start);
		printf("\tResult: %1.7f;\n", sum_up);
		
		//start the sequential algoruthm
		sum_t = 0;
		t_start = MPI_Wtime();

    	for (int i = 0; i < N; ++i)
    	{
        	x = (i + 0.5) / N;
			sum_t += 4 / (1 + x * x);
    	}
		sum_t /= N;
		t_stop = MPI_Wtime();

		//output the sequential algorithm data
		printf("Seq time: %f; ", t_stop - t_start);
		printf("\tResult: %1.7f;\n", sum_t);
	}

    MPI_Finalize();

	return 0;
}

