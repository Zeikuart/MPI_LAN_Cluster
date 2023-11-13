CC = gcc
MPIC = mpicc
MPIR = mpirun
CFLAGS = -Wall
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
	$(MPIR) $(NP) ./$^

clean:
	rm -f main mpiCluster