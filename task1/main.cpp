#include <iostream>
#include <ctime>
#include <omp.h>

int main()
{
    uint32_t N = 1e6;
    double x, seq_sum, par_sum;

    double seq_start, seq_stop, seq_time;
    double par_start, par_stop, par_time;

    uint8_t exec = 10;

    seq_start = omp_get_wtime();
    for (uint8_t j = 0; j < exec; ++j)
    {
		seq_sum = 0;
    	for (uint32_t i = 0; i < N; ++i)
    	{
            x = (i + 0.5) / N;
	    	seq_sum += 4 / (1 + x * x);
    	}
    	seq_sum /= N;
    }
    seq_stop = omp_get_wtime();
    seq_time = (seq_stop - seq_start) / exec;

    par_start = omp_get_wtime();
    for (uint8_t j = 0; j < exec; ++j)
    {
		par_sum = 0;
		#pragma omp parallel for private(x), reduction(+: par_sum)
    	for (uint32_t i = 0; i < N; ++i)
    	{
            x = (i + 0.5) / N;
	    	par_sum = par_sum + 4 / (1 + x * x);
    	}
    	par_sum /= N;
    }
    par_stop = omp_get_wtime();
    par_time = (par_stop - par_start) / exec;

    std::cout.precision(8);
    std::cout << "Method\tAvrTime(sec)\tResult\n";
    std::cout << "Seq\t" << seq_time << '\t';
    std::cout << seq_sum << '\n';
    std::cout << "Par\t" << par_time << '\t';
    std::cout << par_sum << '\n';

    return 0;
}
