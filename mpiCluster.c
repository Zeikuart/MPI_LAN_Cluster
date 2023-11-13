#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define MAX_SIZE 1000
#define SEND_DATA_TAG 2001
#define RETURN_DATA_TAG 2002

// Arrays
int array1[MAX_SIZE];
int array2[MAX_SIZE];

int main(int argc, char *argv[]) {

    // Init Values
    long int totalSum, partialSum;
    MPI_Status status;
    int init, numProcs, pID, rootProcess, aID, numRowsToReceive, avgRowsPerProcess, sender, numRowsReceived, startRow, endRow, numRowsToSend;
    int numRows = 10;

    // Warning handler
    init = 0;
    printf("%d\n", init);

    // Initializer - Childs Creation
    init = MPI_Init(&argc, &argv);

    // Stablishing Root Proecess Rank
    rootProcess = 0;

    // Finding out the IDs and Number of processes
    init = MPI_Comm_rank(MPI_COMM_WORLD, &pID);
    init = MPI_Comm_size(MPI_COMM_WORLD, &numProcs);
    
    

    // printf("Processing! Process %i out of %i processes\n", id, numProcs);

    // If 0 = Root
    if(pID == rootProcess) {
        printf("I'm the Master, process %i out of %i Processes\n", pID, numProcs);

        // Validation
        if(numRows > MAX_SIZE) {

            printf("Too many numbers.\n");
            exit(1);
        }

        // Data per Process
        avgRowsPerProcess = numRows / numProcs;

        // Initialize Array
        for(int i = 0; i < numRows; i++) {

            array1[i] = i + 1;
        }

        // Distribute Array to Slaves; aID = Slaves ID
        for(aID = 1; aID < numProcs; aID++) {

            // Start Value
            startRow = aID * avgRowsPerProcess + 1;

            // End Value
            endRow = (aID + 1) * avgRowsPerProcess;

            // If odd
            if((numRows - endRow) < avgRowsPerProcess) {

                endRow = numRows - 1;
            }

            numRowsToSend = endRow - startRow + 1;

            // Sending Amount of Data
            init = MPI_Send(&numRowsToSend, 1, MPI_INT, aID, SEND_DATA_TAG, MPI_COMM_WORLD);

            // Sending Array Address - Starting Point in Array
            init = MPI_Send(&array1[startRow], numRowsToSend, MPI_INT, aID, SEND_DATA_TAG, MPI_COMM_WORLD);
        }

        // Sum Calculation
        totalSum = 0;
        for(int i = 0; i < avgRowsPerProcess + 1; i++) {
            totalSum += array1[i];
        }

        printf("Total %li calculated by Root\n", totalSum);

        // Partial Sums
        for(aID = 1; aID < numProcs; aID++) {

            init = MPI_Recv(&partialSum, 1, MPI_LONG, MPI_ANY_SOURCE, RETURN_DATA_TAG, MPI_COMM_WORLD, &status);

            sender = status.MPI_SOURCE;

            printf("Partial Sum %li returned from process %i\n", partialSum, sender);

            totalSum += partialSum;
        }

        // Print Total Sum
        printf("The Final Total is: %li\n", totalSum);
    }
    // If Slave
    else {

        printf("I'm a Slave, process %i out of %i Processes\n", pID, numProcs);
        // Receiving Amount of Data
        init = MPI_Recv(&numRowsToReceive, 1, MPI_INT, rootProcess, SEND_DATA_TAG, MPI_COMM_WORLD, &status);

        // Receiving Array Address
        init = MPI_Recv(&array2, numRowsToReceive, MPI_INT, rootProcess, SEND_DATA_TAG, MPI_COMM_WORLD, &status);

        // Amount of Data received
        numRowsReceived = numRowsToReceive;

        // Partial Sum Calculation
        partialSum = 0;
        for(int i = 0; i < numRowsReceived; i++) {

            partialSum += array2[i];
        }

        // Sending Partial Sum
        init = MPI_Send(&partialSum, 1, MPI_LONG, rootProcess, RETURN_DATA_TAG, MPI_COMM_WORLD);
    }

    init = MPI_Finalize();

    return 0;
}