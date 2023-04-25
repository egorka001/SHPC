#!/bin/bash
#SBATCH -p gnu
#SBATCH -N 4 
#SBATCH -n 4 
#SBATCH --ntasks-per-node=8
mpirun xhpl
