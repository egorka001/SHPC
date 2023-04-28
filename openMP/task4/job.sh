#!/bin/bash
#SBATCH -c 8 
#SBATCH -p gnu
export OMP_NUM_THREADS=8
./out
