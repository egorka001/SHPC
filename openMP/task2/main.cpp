#include <iostream>
#include <iomanip>
#include <ctime>
#include <omp.h>

double pi_calc(uint32_t N, uint8_t exec)
{
	double par_sum, x;
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
	return par_sum;
}

int main()
{
    uint8_t exec = 10;
    uint32_t N_small = 1e2, N_big = 1e6;
    double par_sum, par_start, par_stop, par_time;

    par_start = omp_get_wtime();
	par_sum = pi_calc(N_small, exec);
    par_stop = omp_get_wtime();
    par_time = (par_stop - par_start) / exec;

    std::cout << std::setprecision(8) << std::fixed;
    std::cout << "N\tAvrTime(sec)\tResult\n";
    std::cout << "1e2\t" << par_time << '\t' << par_sum << '\n';

    par_start = omp_get_wtime();
	par_sum = pi_calc(N_big, exec);
    par_stop = omp_get_wtime();
    par_time = (par_stop - par_start) / exec;

    std::cout << "1e6\t" << par_time << '\t' << par_sum << '\n';

    return 0;
}
