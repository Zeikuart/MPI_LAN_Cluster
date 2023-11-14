#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <omp.h>

// MAX_COLS Arrays of 1000000 elements max each
#define MAX_SIZE 1000000
#define MAX_COLS 7
#define SEND_DATA_TAG 2001
#define RETURN_DATA_TAG 2002

// Numes of Files
#define TIMES 50

// 10 Samples per file
#define NUM_SAMPLES 10

// Initial Value of elements
#define ROWS 1000

//** Arrays **//

// Master Matrix
int matrix[MAX_COLS][MAX_SIZE];

// Slave Array
int array[MAX_SIZE];

// Temp
int tmp[MAX_SIZE];

int main(int argc, char *argv[]) {

    double start_time, run_time;

    // Init Values
    long int totalSum, partialSum;
    MPI_Status status;
    int init, numProcs, pID, rootProcess, aID, numRowsToReceive, avgRowsPerProcess, sender, numRowsReceived, startRow, endRow, numRowsToSend;

    // Size = 1000 elements per array
    int numRows = 1000;

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

    // Number of Files
    for (int i = 0; i < TIMES; i++) {

        char filename[100];
        snprintf(filename, sizeof(filename), "results/mpi_loop_%02d.txt", i);

        // Num of elements in Row = 1000 * i
        for (int iter = 1; iter <= NUM_SAMPLES; iter++) {

            // If 0 = Root
            if (pID == rootProcess) {

                // Record the start time
                start_time = omp_get_wtime();
                // printf("I'm the Master, process %i out of %i Processes\n", pID, numProcs);

                // Validation
                if (numRows > MAX_SIZE) {

                    printf("Too many numbers.\n");
                    exit(1);
                }

                // Data per Process
                avgRowsPerProcess = MAX_COLS / numProcs;

                // Initialize Array
                for (int i = 0; i < MAX_COLS; i++) {

                    // Cols * Rows
                    for (int j = 0; j < numRows * iter; j++) {

                        // Starting from 1
                        matrix[i][j] = i * numRows * iter + j + 1;
                    }
                }

                // Print Matrix
                // for (int i = 0; i < MAX_COLS; ++i) {
                //     for (int j = 0; j < numRows * iter; ++j) {
                //         printf("%3d ", matrix[i][j]);
                //     }
                //     printf("\n");
                // }

                // Distribute Matrix to Slaves; aID = Slaves ID
                for (aID = 1; aID < numProcs; aID++) {

                    // Start Value
                    startRow = aID * avgRowsPerProcess + 1;

                    // End Value
                    endRow = (aID + 1) * avgRowsPerProcess;

                    // If odd
                    if ((numRows - endRow) < avgRowsPerProcess) {

                        endRow = numRows - 1;
                    }

                    numRowsToSend = (endRow - startRow + 1) * numRows * iter;

                    printf("Numbers to Sum in Slave: %i\n", numRowsToSend);

                    // Sending Amount of Data
                    init = MPI_Send(&numRowsToSend, 1, MPI_INT, aID, SEND_DATA_TAG, MPI_COMM_WORLD);

                    for (int i = 0; i < avgRowsPerProcess; i++) {

                        for (int k = 0; k < numRows  * iter; k++)
                        {

                            tmp[i * numRows * iter + k] = matrix[startRow + i][k];
                        }
                    }

                    // Print Matrix
                    // printf("Array Temp:\n");
                    // for (int j = 0; j < numRows * avgRowsPerProcess; ++j) {

                    //     printf("%3d ", tmp[j]);
                    // }
                    // printf("\n");

                    // Sending Array Address - Starting Point in Array
                    init = MPI_Send(&tmp[0], numRowsToSend, MPI_INT, aID, SEND_DATA_TAG, MPI_COMM_WORLD);
                }

                // Sum Calculation
                totalSum = 0;

                for (int i = 0; i < avgRowsPerProcess + 1; i++) {

                    for (int j = 0; j < numRows  * iter; j++)
                    {

                        totalSum += matrix[i][j];
                    }
                }

                printf("Total %li calculated by Root\n", totalSum);

                // Partial Sums
                for (aID = 1; aID < numProcs; aID++) {

                    init = MPI_Recv(&partialSum, 1, MPI_LONG, MPI_ANY_SOURCE, RETURN_DATA_TAG, MPI_COMM_WORLD, &status);

                    sender = status.MPI_SOURCE;

                    printf("Partial Sum %li returned from process %i\n", partialSum, sender);

                    totalSum += partialSum;
                }

                // Record the end time
                run_time = omp_get_wtime() - start_time;

                // Print Total Sum
                // printf("The Final Total is: %li\n", totalSum);
                printf("computed value = %li in %3.15f seconds\n", totalSum, run_time);

                FILE *file = fopen(filename, "a");

                if (file) {

                    fprintf(file, "num_values = %d computed value = %ld in %3.15f seconds\n", (numRows * iter * MAX_COLS), totalSum, run_time);

                    fclose(file);

                    printf("Execution %d completed. Results stored in %s\n", i, filename);

                } else {
                
                    printf("Error opening file %s for writing.\n", filename);
                }
            }

            // If Slave
            else {

                // printf("I'm a Slave, process %i out of %i Processes\n", pID, numProcs);

                // Receiving Amount of Data
                init = MPI_Recv(&numRowsToReceive, 1, MPI_INT, rootProcess, SEND_DATA_TAG, MPI_COMM_WORLD, &status);

                // Receiving Array Address
                init = MPI_Recv(&array, numRowsToReceive, MPI_INT, rootProcess, SEND_DATA_TAG, MPI_COMM_WORLD, &status);

                // Amount of Data received
                numRowsReceived = numRowsToReceive;

                // printf("Numbers received from Master: %i\n", numRowsReceived);

                // Partial Sum Calculation
                partialSum = 0;
                for (int i = 0; i < numRowsReceived; i++) {

                    partialSum += array[i];
                    // HEre
                    // printf("%3d", array[i]);
                }
                // Here
                // printf("\n");

                // Sending Partial Sum
                init = MPI_Send(&partialSum, 1, MPI_LONG, rootProcess, RETURN_DATA_TAG, MPI_COMM_WORLD);
            }

            
        }
    }
    init = MPI_Finalize();
    // printf("Finished!\n");
    return 0;
}