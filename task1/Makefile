all: compil execute

compil:
	gcc main.cpp -fopenmp -lstdc++ -o out

execute:
	./out

launch:
	sbatch job.sh

