all: compil start

compil: main.c
	mpicc main.c -o main

start:
	mpirun main

clean:
	rm -f main 
