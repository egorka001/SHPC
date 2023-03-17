#!/bin/bash
#SBATCH -c 4
#SBATCH -p gnu
export OMP_NUM_THREADS=4
./out
