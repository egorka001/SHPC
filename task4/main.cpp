#include <iostream>
#include <ctime>
#include <omp.h>

bool check(int32_t* array, uint32_t size)
{
    for(uint32_t i = 0; i < size - 1; ++i)
        if(array[i] > array[i + 1])
            return 0;
    return 1;

}

int32_t partition(int32_t* array, int32_t begin, int32_t end)
{
    int32_t pivot = array[begin];

    int32_t count = 0;
    for (int32_t i = begin + 1; i <= end; i++)
        if (array[i] <= pivot)
            count++;

    int32_t pivot_idx = begin + count;
    std::swap(array[pivot_idx], array[begin]);

    int32_t i = begin, j = end;
    while (i < pivot_idx && j > pivot_idx) 
    {
        while (array[i] <= pivot)
            i++;

        while (array[j] > pivot)
            j--;

        if (i < pivot_idx && j > pivot_idx)
            std::swap(array[i++], array[j--]);
    }

    return pivot_idx;
}

void quick_sort(int32_t* array, int32_t begin, int32_t end)
{
    if (begin >= end)
        return;
 
    int32_t p = partition(array, begin, end);
	#pragma omp task 
    quick_sort(array, begin, p - 1);
	#pragma omp task 
    quick_sort(array, p + 1, end);
}

int main()
{
    uint32_t size = 1e6;

    int32_t* array = new int32_t[size];

	double start, stop, ex_time;
    
    srand(time(NULL));
    for(uint32_t i = 0; i < size; ++i)
        array[i] = rand() % 1000 - 500;

	start = omp_get_wtime();
	#pragma omp parallel //запускаем распараллелилование
	{
		#pragma omp single //говорим, что первый quick_sort делаем один раз
    	quick_sort(array, 0, size - 1);
	}
	stop = omp_get_wtime();
	ex_time = stop - start;

	if(check(array, size))
    	std::cout << "TimeSort: " << ex_time << " seconds\n";

    delete array;

    return 0;
}
