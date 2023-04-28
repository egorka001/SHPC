#!/bin/sh
#SBATCH -p gnu
#SBATCH -N 2 
module load openmpi
mpirun main 
