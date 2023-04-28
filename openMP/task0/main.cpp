#include <iostream>
#include <ctime>
#include <omp.h>

int main()
{
    uint32_t size = 1e6;
    int32_t* array = new int32_t[size];

    int32_t sum_seq = 0;
    int32_t sum_par = 0;
    int32_t sum_par_red = 0;

    double start_seq, stop_seq, seq_time;
    double start_par, stop_par, par_time;
    double start_par_red, stop_par_red, par_red_time;

    uint8_t exec = 10;

    for (uint32_t i = 0; i < size; ++i)
		array[i] = 1;

    start_seq = omp_get_wtime();
    for(uint8_t j = 0; j < exec; ++j)
    {
        for (uint32_t i = 0; i < size; ++i)
	    	sum_seq += array[i];
    }
    stop_seq = omp_get_wtime();

    start_par = omp_get_wtime();
    for(uint8_t j = 0; j < exec; ++j)
    {
        #pragma omp parallel for 
    	for (uint32_t i = 0; i < size; ++i)
		{
	    	#pragma omp atomic
	    	sum_par += array[i];
		}
    }
    stop_par = omp_get_wtime();
 
    start_par_red = omp_get_wtime();
    for(uint8_t j = 0; j < exec; ++j)
    {
        #pragma omp parallel for reduction(+:sum_par_red)
    	for (uint32_t i = 0; i < size; ++i)
	    	sum_par_red = sum_par_red + array[i];
    }
    stop_par_red = omp_get_wtime();

    seq_time = (stop_seq - start_seq) / exec;
    par_time = (stop_par - start_par) / exec;
    par_red_time = (stop_par_red - start_par_red) / exec;

    std::cout << "Method\tAvrTime(seconds)\n";
    std::cout << "Seq:\t" << seq_time << '\n';
    std::cout << "Par:\t" << par_time << '\n';
    std::cout << "Red:\t" << par_red_time << '\n';

    delete array;

    return 0;
}
