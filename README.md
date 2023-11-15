# MPI LAN Cluster

This homework is a small introduction to the Message Passing Interface(MPI). MPI, is a library that provides you with some routines and diretives to perform parallel programs either in C or Fortran, in this homework's case, with C. We used Ubuntu 22 as Master and Ubuntu 20 as Slave.

## Requirements

- Build Tools: 
    * sudo apt-get install build-essential

- OpenMP Library: 
    * sudo apt-get install libomp-dev

- MPICH:
    * We used mpich-4.1.2
    * Download lastest version of mpich here https://www.mpich.org/
    * Open directory: cd /mpich-4.1.2
    * Execute "./configure --disable-fortran"
    * Run Makefiles: make; sudo make install
    * Check version: mpiexec --version

- Python Plot Library:
    * pip install plotly --upgrade
    * Additional dependencies: pip install ipywidgets

- Virtual Machine:
    * In this case, we used Ubuntu 20 as a slave.

- SSH:
    * sudo apt­-get install openssh-server
    * sudo apt­-get install openssh-client

- NFS in Master:
    * sudo apt-get install nfs-kernel-server

- NFS in Slave:
    * sudo apt-get install nfs-common

## Compilation

For the compilation of our program, this happens automatically by running the following command in Slave(Worker1): *'sudo make'*.
We ran the command from worker1(Slave) because of being an older version of Ubuntu(20) than the Master's(22), could throw some errors due to some incompatibilty if we compiled it from Master.

## Execution

Just run: *'sudo make run'*. This can be perfectly run from Master.

## Visualization

- Run the Python script:
    * sudo python3 plotter.py
    * Find the html file generated in the previous step and open it, this will display a graph.