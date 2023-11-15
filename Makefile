CC = gcc
MPIC = mpicc
MPIR = mpirun
CFLAGS = -Wall -fopenmp
OPT = -O2
NP = -np 2

.PHONY: all clean

all: mpiCluster

#main: main.c
#	$(MPIC) -o $@ $^ $(CFLAGS)

mpiCluster: mpiCluster.c
	$(MPIC) -o $@ $^ $(CFLAGS)

#run: main
#	$(MPIR) $(NP) ./$^

run: mpiCluster
	$(MPIR) $(NP) -hosts master,worker1 ./$^

clean:
	rm -f main mpiCluster results/mpi_loop_*.txt plotly_graph.html