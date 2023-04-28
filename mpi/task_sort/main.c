#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

void merge(int* array, int left, int mid, int right)
{
    int i, j, k;
    int n1 = mid - left + 1;
    int n2 = right - mid;
 
    int* L = (int*)malloc(n1 * sizeof(int));
    int* R = (int*)malloc(n2 * sizeof(int));
 
    for (i = 0; i < n1; i++)
        L[i] = array[left + i]; 
	for (j = 0; j < n2; j++) 
		R[j] = array[mid + 1 + j];
 
    i = 0; 
    j = 0;
    k = left; 

    while (i < n1 && j < n2) 
	{
        if (L[i] <= R[j]) 
		{
            array[k] = L[i];
            i++;
        }
        else 
		{
            array[k] = R[j];
            j++;
        }
        k++;
    }
 
    while (i < n1) 
	{
        array[k] = L[i];
        i++;
        k++;
    }
 
    while (j < n2) 
	{
        array[k] = R[j];
        j++;
        k++;
    }
	
	free(L);
	free(R);
}

void mergeSort(int* array, int left, int right)
{
    if (left < right) 
	{
        int mid = left + (right - left) / 2;
        mergeSort(array, left, mid);
        mergeSort(array, mid + 1, right);
        merge(array, left, mid, right);
    }
}

bool check(int* array, int size)
{
    for(int i = 0; i < size - 1; ++i)
        if(array[i] > array[i + 1])
            return 0;
    return 1;
}

void out_a(int* array, int size, int rank)
{
	printf("%d | ", rank);
	for (int i = 0; i < size; ++i)
		printf("%d ", array[i]);
	printf("\n");

}

int main(int argc, char** argv) 
{
    int nranks, rank;

	int size = 1 << 20; 
    int* array_par_buf;
	int* array_seq;
	int div, color;

	double t_start_seq, t_stop_seq;
	double t_start_par, t_stop_par;

	MPI_Comm SORT_COMM;

    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &nranks);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if (rank == 0)
	{
    	array_seq = (int*)malloc(size * sizeof(int));
    	array_par_buf = (int*)malloc(size * sizeof(int));

    	srandom(time(NULL));
    	for (int i = 0; i < size; ++i)
		{
        	array_seq[i] = random() % 1000 - 500;
			array_par_buf[i] = array_seq[i];
		}
	}

	MPI_Barrier(MPI_COMM_WORLD);
	t_start_par = MPI_Wtime();

	int subarray_size = size / nranks;
	int* subarray = (int*)malloc(subarray_size * sizeof(int));	

	MPI_Scatter(array_par_buf, subarray_size, MPI_INT, 
	    		subarray, subarray_size, MPI_INT, 0, MPI_COMM_WORLD);

	mergeSort(subarray, 0, subarray_size - 1);
	
	MPI_Gather(subarray, subarray_size, MPI_INT, 
    		   array_par_buf, subarray_size, MPI_INT, 0, MPI_COMM_WORLD);

	free(subarray);

	div = 2;
	while(size != subarray_size * 2)
	{
		color = rank % div;
		subarray_size *= 2;
		div *= 2;

		MPI_Comm_split(MPI_COMM_WORLD, color, rank, &SORT_COMM);

		if(color == 0)
		{
		subarray = (int*)malloc(subarray_size * sizeof(int));	

		MPI_Scatter(array_par_buf, subarray_size, MPI_INT, 
	    			subarray, subarray_size, MPI_INT, 0, SORT_COMM);

		merge(subarray, 0, subarray_size / 2 - 1, subarray_size - 1);

		MPI_Gather(subarray, subarray_size, MPI_INT, 
        		  array_par_buf, subarray_size, MPI_INT, 0, SORT_COMM);

		free(subarray);
		}
	}
	if (rank == 0)
		merge(array_par_buf, 0, size / 2 - 1, size - 1);

	MPI_Barrier(MPI_COMM_WORLD);
	t_stop_par = MPI_Wtime();

	if (rank == 0)
	{	
		t_start_seq = MPI_Wtime();
		mergeSort(array_seq, 0, size - 1);
		t_stop_seq = MPI_Wtime();
		
		if(check(array_seq, size) && check(array_par_buf, size))
		{
			printf("RANKS: %d\n", nranks);
			printf("Array size: %d\n", size);
			printf("Seq time: %f\n", t_stop_seq - t_start_seq);
			printf("Par time: %f\n", t_stop_par - t_start_par);
		}	

		free(array_seq);
		free(array_par_buf);
	}

    MPI_Finalize();

	return 0;
}

