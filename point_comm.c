#include<stdio.h> // Standard IO
#include<stdlib.h> // Standard Library
#include<mpi.h>


void calculate_bandwidth(double average_time, int data_size) {
    double bandwidth;
    char *unit = "B/s";
//    printf("%lf\n", average_time);
    bandwidth = (double) data_size / average_time;
    if (bandwidth >= 1024.) {
        bandwidth /= 1024.;
        unit = "KB/s";
        if (bandwidth >= 1024.) {
            bandwidth /= 1024.;
            unit = "MB/s";
            if (bandwidth >= 1024.) {
                bandwidth /= 1024.;
                unit = "GB/s";
            }
        }
    }
    printf("%lf %s", bandwidth, unit);
}

int main(int argc, char *argv[]) {


    int n, i, j, k, my_rank, nprocs;
    int data_size, ntimes = atoi(argv[1]);
    double start_time, end_time, time_cost, average_time;
    char *p;// dynamically allocate memory




    MPI_Comm_size(MPI_COMM_WORLD, &nprocs); // get the number of processes
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);// get the local rank
    MPI_Status status;
    if (my_rank == 0) {
        for (i = 0; i < argc; i++) {
            printf("%s\n", argv[i]);
        }
    }
    for (n = 0; n < argc - 2; n++) {
        data_size = atoi(argv[2 + n]) * 1024;
        p = (char *) malloc(data_size);
        if (!p) {
            printf("Error when allocating %d memory\n", data_size);
            exit(1);
        }

        if (my_rank == 0) { printf("Benchmark with %dkb data\n", data_size / 1024); }
        for (i = 0; i < nprocs; i++) {
            for (j = 0; j < nprocs; j++) {
                MPI_Barrier(MPI_COMM_WORLD);
                if (i == j || (my_rank != i && my_rank != j)) {
                    continue;
                }
                start_time = MPI_Wtime(); //get the wall time from MPI
                // Repeat communication for k times
                for (k = 1; k <= ntimes; k++) {
                    if (my_rank == i) {
                        MPI_Send(p, data_size, MPI_CHAR, j, k, MPI_COMM_WORLD);
                    }
                    if (my_rank == j) {
                        MPI_Recv(p, data_size, MPI_CHAR, i, k, MPI_COMM_WORLD, &status);
                    }
                }
                end_time = MPI_Wtime();
                time_cost = end_time - start_time;
                if (my_rank == i) {
                    average_time = time_cost / ntimes;
                    printf("%d -> %d Average bandwidth ", i, j);
                    calculate_bandwidth(average_time, data_size);
                    printf("\n");
                    fflush(stdout);
                }
            }
        }
        free(p);
    }

    MPI_Finalize(); // Terminates MPI execution environment
    return 0;
}