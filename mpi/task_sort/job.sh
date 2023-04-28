#!/bin/sh
#SBATCH -p gnu
#SBATCH -N 4 
module load openmpi
mpirun main 
